/* packet-awds.c
 * Routines for Agent Ad-Hoc Wireless Distribution Service (AWDS) Protocol disassembly
 * RFC 2257
 *
 * $Id: packet-awds.c 21253 2007-08-30 18:00:00Z sfisher $
 *
 * Copyright (c) 2007 by Christian Fackroth <cfackrot@cs.uni-magdeburg.de>
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1999 Gerald Combs
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string.h>

#include <epan/packet.h>
#include <epan/prefs.h>

#define AWDS_BEACON   0x00
#define AWDS_FLOOD    0x01
#define AWDS_UNICAST  0x02
#define AWDS_FLOW     0x03

#define AWDS_TOPO     0x00
#define AWDS_CRYPTO   0x63
#define AWDS_CAP_LINK 0x12
#define AWDS_CAP_DATA 0x62

/* Forward reference */
void proto_register_awds();
void proto_reg_handoff_awds();
void dissect_awds(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree);

/* General stuff */
static gint proto_awds = -1;
static gint ett_awds = -1;
static gint ett_awds_subtype = -1;
static guint16 global_awds_id = 0x8333;
static dissector_handle_t eth_handle;
static dissector_handle_t awds_handle;
//static dissector_handle_t data_handle;

/* General packet types */
static gint hf_awds_type = -1;
static const value_string awds_types[] = {
  {AWDS_BEACON, "Beacon"},
  {AWDS_FLOOD, "Flood"},
  {AWDS_UNICAST, "Unicast"},
  {AWDS_FLOW, "Flow"},
  {0, "Unknown"}
};

/* General packet subtypes */
static gint hf_awds_subtype = -1;
static const value_string awds_subtypes[] = {
  {AWDS_TOPO, "Topology"},
  {AWDS_CRYPTO, "Cryptographic"},
  {AWDS_CAP_LINK, "LinkQuality Probe"},
  {AWDS_CAP_DATA, "Data"},
  {0, "Unknown"}
};

/* Packet flags */
static gint hf_awds_flags = -1;
static gint ett_awds_flags = -1;
static gint hf_awds_flag_0 = -1;
static const true_false_string awds_flag_0 = {
  "Enabled",
  "Not Enabled"
};
static gint hf_awds_flag_1 = -1;
static const true_false_string awds_flag_1 = {
  "Enabled",
  "Not Enabled"
};
static gint hf_awds_flag_2 = -1;
static const true_false_string awds_flag_2 = {
  "Enabled",
  "Not Enabled"
};
static gint hf_awds_flag_3 = -1;
static const true_false_string awds_flag_3 = {
  "Enabled",
  "Not Enabled"
};
static gint hf_awds_flag_4 = -1;
static const true_false_string awds_flag_4 = {
  "Enabled",
  "Not Enabled"
};
static gint hf_awds_flag_5 = -1;
static const true_false_string awds_flag_5 = {
  "Enabled",
  "Not Enabled"
};

/* Source MAC-address */
static gint hf_awds_src_mac_addr = -1;

/* Sequence number */
static gint hf_awds_seq_no = -1;

/* Beacon interval */
static gint hf_awds_beacon_int = -1;

/* Beacon Non-mulipoint relay counter */
static gint hf_awds_beacon_nmpr_cnt = -1;

/* Beacon Mulipoint relay counter */
static gint hf_awds_beacon_mpr_cnt = -1;

/* Beacon Non-multipoint relay list */
static gint hf_awds_beacon_nmbr_lst = -1;
static gint ett_awds_beacon_nmbr_lst = -1;
static gint hf_awds_beacon_nmbr_mac_addr = -1;

/* Beacon Multipoint relay list */
static gint hf_awds_beacon_mbr_lst = -1;
static gint ett_awds_beacon_mbr_lst = -1;
static gint hf_awds_beacon_mbr_mac_addr = -1;

/* Flood Lasthop MAC-address */
static gint hf_awds_flood_lasthop_mac_addr = -1;

/* Flood TTL value */
static gint hf_awds_flood_ttl = -1;

/* Unicast destination MAC-address */
static gint hf_awds_unicast_dest_mac_addr = -1;

/* Unicast Nexthop MAC-address */
static gint hf_awds_unicast_nexthop_mac_addr = -1;

/* Unicast TTL value */
static gint hf_awds_unicast_ttl = -1;

/* Topology validity value */
static gint hf_awds_topo_validity = -1;

/* Topology link counter */
static gint hf_awds_topo_link_cnt = -1;

/* Topology link list */
static gint hf_awds_topo_link_lst = -1;
static gint ett_awds_topo_link_lst = -1;
static gint ett_awds_topo_link_entry = -1;
static gint hf_awds_topo_link_mac_addr = -1;
static gint hf_awds_topo_link_cost = -1;

/* Topology Node-name length */
static gint hf_awds_topo_node_name_len = -1;

/* Topology Node-name */
static gint hf_awds_topo_node_name = -1;

/* Cryptographic initialisation vector */
static gint hf_awds_crypto_iv = -1;

/* Cryptographic nonce */
static gint hf_awds_crypto_nonce = -1;


void proto_register_awds(void){
  /* Setup protocol information array */
  static hf_register_info hf[] = {
    /* For all AWDS-packets */
    {&hf_awds_type,
      {"Type", "awds.type",
       FT_UINT8, BASE_DEC,
       VALS(&awds_types), 0x03,
       NULL, HFILL}},
    {&hf_awds_flags,
      {"Flags", "awds.flags",
       FT_NONE, BASE_NONE,
       NULL, 0,
       NULL, HFILL}},
    {&hf_awds_flag_0,
      {"Unknown", "awds.flags.one",
       FT_BOOLEAN, 8,
       TFS(&awds_flag_0), 0x04,
       "Unknown-Flag", HFILL}},
    {&hf_awds_flag_1,
      {"Unknown", "awds.flags.two",
       FT_BOOLEAN, 8,
       TFS(&awds_flag_1), 0x08,
       "Unknown-Flag", HFILL}},
    {&hf_awds_flag_2,
      {"Unknown", "awds.flags.three",
       FT_BOOLEAN, 8,
       TFS(&awds_flag_2), 0x10,
       "Unknown-Flag", HFILL}},
    {&hf_awds_flag_3,
      {"Unknown", "awds.flags.four",
       FT_BOOLEAN, 8,
       TFS(&awds_flag_3), 0x20,
       "Unknown-Flag", HFILL}},
    {&hf_awds_flag_4,
      {"Unknown", "awds.flags.five",
       FT_BOOLEAN, 8,
       TFS(&awds_flag_4), 0x40,
       "Unknown-Flag", HFILL}},
    {&hf_awds_flag_5,
      {"Trace", "awds.flags.trace",
       FT_BOOLEAN, 8,
       TFS(&awds_flag_5), 0x80,
       "Trace-Flag", HFILL}},
    {&hf_awds_src_mac_addr,
      {"Source address", "awds.sa",
       FT_ETHER, BASE_NONE,
       NULL, 0,
       "Source MAC adress", HFILL}},
    {&hf_awds_seq_no,
      {"Sequence number", "awds.seqno",
       FT_UINT16, BASE_DEC,
       NULL, 0,
       NULL, HFILL}},
    /* For Beacon-packets */
    {&hf_awds_beacon_int,
      {"Beacon Interval", "awds.bint",
       FT_UINT16, BASE_DEC,
       NULL, 0,
       "Beacon Interval in seconds", HFILL}},
    {&hf_awds_beacon_nmpr_cnt,
      {"Non-MPR count", "awds.beacon.nmpr_cnt",
       FT_UINT8, BASE_DEC,
       NULL, 0,
       NULL, HFILL}},
    {&hf_awds_beacon_mpr_cnt,
      {"MPR count", "awds.beacon.mpr_cnt",
       FT_UINT8, BASE_DEC,
       NULL, 0,
       NULL, HFILL}},
    {&hf_awds_beacon_nmbr_lst,
      {"Non-MPR list", "awds.beacon.nmpr_lst",
      FT_NONE, BASE_NONE,
      NULL, 0,
      NULL, HFILL}},
    {&hf_awds_beacon_nmbr_mac_addr,
      {"Address", "awds.beacon.nmpr_mac",
       FT_ETHER, BASE_NONE,
       NULL, 0,
       "MAC adress", HFILL}},
    {&hf_awds_beacon_mbr_lst,
      {"MPR list", "awds.beacon.mpr_lst",
      FT_NONE, BASE_NONE,
      NULL, 0,
      NULL, HFILL}},
    {&hf_awds_beacon_mbr_mac_addr,
      {"Address", "awds.beacon.mpr_mac",
       FT_ETHER, BASE_NONE,
       NULL, 0,
       "MAC adress", HFILL}},
    /* For Flood-packets */
    {&hf_awds_flood_lasthop_mac_addr,
      {"Last-hop address", "awds.flood.lasthop_mac",
       FT_ETHER, BASE_NONE,
       NULL, 0,
       "Lasthop MAC adress", HFILL}},
    {&hf_awds_flood_ttl,
      {"TTL", "awds.flood.ttl",
       FT_UINT8, BASE_DEC,
       NULL, 0,
       "Time To Live (number of hops)", HFILL}},
    /* For Unicast-packets */
    {&hf_awds_unicast_dest_mac_addr,
      {"Destination address", "awds.unicast.da",
       FT_ETHER, BASE_NONE,
       NULL, 0,
       "Destination MAC adress", HFILL}},
    {&hf_awds_unicast_nexthop_mac_addr,
      {"Next-hop address", "awds.unicast.nexthop",
       FT_ETHER, BASE_NONE,
       NULL, 0,
       "Next-hop MAC adress", HFILL}},
    {&hf_awds_unicast_ttl,
      {"TTL", "awds.unicast.ttl",
       FT_UINT8, BASE_DEC,
       NULL, 0,
       "Time To Live (number of hops)", HFILL}},
    /* For Subtype-packets */
    {&hf_awds_subtype,
      {"SubType", "awds.subtype",
       FT_UINT8, BASE_DEC,
       VALS(&awds_subtypes), 0,
       NULL, HFILL}},
    /* For Topo-packets */
    {&hf_awds_topo_validity,
      {"Validity", "awds.topo.validity",
       FT_UINT32, BASE_DEC,
       NULL, 0,
       NULL, HFILL}},
    {&hf_awds_topo_link_cnt,
      {"Link count", "awds.topo.link_cnt",
       FT_UINT8, BASE_DEC,
       NULL, 0,
       NULL, HFILL}},
    {&hf_awds_topo_link_lst,
      {"Link list", "awds.topo.link_lst",
      FT_NONE, BASE_NONE,
      NULL, 0,
      "List of neighbour nodes", HFILL}},
    {&hf_awds_topo_link_mac_addr,
      {"Address", "awds.topo.link_mac",
       FT_ETHER, BASE_NONE,
       NULL, 0,
       "MAC adress", HFILL}},
    {&hf_awds_topo_link_cost,
      {"Link Cost", "awds.topo.link_cost",
       FT_UINT16, BASE_DEC,
       NULL, 0,
       NULL, HFILL}},
    {&hf_awds_topo_node_name_len,
      {"Node name length", "awds.topo.node_name_len",
       FT_UINT8, BASE_DEC,
       NULL, 0,
       NULL, HFILL}},
    {&hf_awds_topo_node_name,
      {"Node name", "awds.topo.node_name",
       FT_STRING, BASE_NONE,
       NULL, 0,
       NULL, HFILL}},
    /* For Crypto-packets */
    {&hf_awds_crypto_iv,
      {"IV", "awds.crypto.iv",
       FT_UINT64, BASE_HEX,
       NULL, 0,
       "Initialization vector", HFILL}},
    {&hf_awds_crypto_nonce,
      {"Nonce", "awds.crypto.nonce",
       FT_UINT64, BASE_HEX,
       NULL, 0,
       NULL, HFILL}}
  };

  /* Setup protocol subtree array */
  static gint *ett[] = {
    &ett_awds
  };

  /* Setup protocol subtype subtree array */
  static gint *ett_subtype[] = {
    &ett_awds_subtype
  };

  /* Setup flags subtree array */
  static gint *ett_flags[] = {
    &ett_awds_flags
  };

  /* Setup beacon nmpr subtree array */
  static gint *ett_beacon_nmpr_lst[] = {
    &ett_awds_beacon_nmbr_lst
  };

  /* Setup beacon mpr subtree array */
  static gint *ett_beacon_mpr_lst[] = {
    &ett_awds_beacon_mbr_lst
  };

  /* Setup topo link subtree array */
  static gint *ett_topo_link_lst[] = {
    &ett_awds_topo_link_lst
  };

  /* Setup topo link subtree entry array */
  static gint *ett_topo_link_entry[] = {
    &ett_awds_topo_link_entry
  };

  /* Registering data structures */
  proto_register_field_array(proto_awds, hf, array_length(hf));
  proto_register_subtree_array(ett, array_length(ett));
  proto_register_subtree_array(ett_subtype, array_length(ett_subtype));
  proto_register_subtree_array(ett_flags, array_length(ett_flags));
  proto_register_subtree_array(ett_beacon_nmpr_lst, array_length(ett_beacon_nmpr_lst));
  proto_register_subtree_array(ett_beacon_mpr_lst, array_length(ett_beacon_mpr_lst));
  proto_register_subtree_array(ett_topo_link_lst, array_length(ett_topo_link_lst));
  proto_register_subtree_array(ett_topo_link_entry, array_length(ett_topo_link_entry));

  /* Registering protocol */
  if(proto_awds == -1){
    proto_awds = proto_register_protocol(
      "AWDS-Protocol", /* name */
      "AWDS",          /* short name */
      "awds"           /* abbrev */
    );
  } //of if
} //of proto_register_awds

void proto_reg_handoff_awds(void){
  static int initialized = FALSE;

  if(!initialized){
    //data_handle = find_dissector("data");
    eth_handle = find_dissector("eth_withoutfcs");

    awds_handle = create_dissector_handle(dissect_awds, proto_awds);
    dissector_add("ethertype", global_awds_id, awds_handle);
    initialized = TRUE;
  } else {
    dissector_delete("ethertype", global_awds_id, awds_handle);
    initialized = FALSE;
  } //of if-else
} //of proto_reg_handoff_awds

static void dissect_awds_beacon(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, gint offset){
  guint8 mbr_cnt = 0, nmbr_cnt = 0;
  guint16 interval = 0, i;
  proto_item *awds_item_nmpr = NULL, *awds_item_mpr = NULL;
  proto_tree *awds_tree_nmpr = NULL, *awds_tree_mpr = NULL;

  /* Decode beacon interval */
  interval = tvb_get_ntohs(tvb, offset);
  proto_tree_add_item(tree, hf_awds_beacon_int, tvb, offset, 2, FALSE);
  offset += 2;

  /* Decode non-multipoint relay counter */
  nmbr_cnt = tvb_get_guint8(tvb, offset);
  proto_tree_add_item(tree, hf_awds_beacon_nmpr_cnt, tvb, offset, 1, FALSE);
  offset++;

  /* Decode multipoint relay counter */
  mbr_cnt = tvb_get_guint8(tvb, offset);
  proto_tree_add_item(tree, hf_awds_beacon_mpr_cnt, tvb, offset, 1, FALSE);
  offset++;

  /* Fill information column */
  if(check_col(pinfo->cinfo, COL_INFO))
    col_add_fstr(pinfo->cinfo, COL_INFO, "Beacon (int=%d, nonmprcnt=%d, mprcnt=%d)", interval, nmbr_cnt, mbr_cnt);

  /* Decode non-multipoint relay list if it exists */
  if(nmbr_cnt){
    awds_item_nmpr = proto_tree_add_item(tree, hf_awds_beacon_nmbr_lst, tvb, offset, 6 * nmbr_cnt, FALSE);
    awds_tree_nmpr = proto_item_add_subtree(awds_item_nmpr, ett_awds_beacon_nmbr_lst);
    for(i = 0; i < nmbr_cnt; i++){
      proto_tree_add_item(awds_tree_nmpr, hf_awds_beacon_nmbr_mac_addr, tvb, offset, 6, FALSE);
      offset += 6;
    } //of for
  } //of if

  /* Decode multipoint relay list if it exists */
  if(mbr_cnt){
    awds_item_mpr = proto_tree_add_item(tree, hf_awds_beacon_mbr_lst, tvb, offset, 6 * mbr_cnt, FALSE);
    awds_tree_mpr = proto_item_add_subtree(awds_item_mpr, ett_awds_beacon_mbr_lst);
    for(i = 0; i < mbr_cnt; i++){
      proto_tree_add_item(awds_tree_mpr, hf_awds_beacon_mbr_mac_addr, tvb, offset, 6, FALSE);
      offset += 6;
    } //of for
  } //of if
} //of dissect_awds_beacon

static void dissect_awds_topo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, gint offset){
  guint8 link_cnt = 0, name_len = 0;
  guint16 cost = 0;
  guint8 address[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  guint16 i;
  proto_item *awds_item_link = NULL, *awds_item_sublink = NULL;
  proto_tree *awds_tree_link = NULL, *awds_tree_sublink = NULL;

  /* Decode validity */
  proto_tree_add_item(tree, hf_awds_topo_validity, tvb, offset, 4, FALSE);
  offset += 4;

  /* Decode link counter */
  link_cnt = tvb_get_guint8(tvb, offset);
  proto_tree_add_item(tree, hf_awds_topo_link_cnt, tvb, offset, 1, FALSE);
  offset++;

  /* Decode link list if it exists */
  if(link_cnt){
    awds_item_link = proto_tree_add_item(tree, hf_awds_topo_link_lst, tvb, offset, 8 * link_cnt, FALSE);
    awds_tree_link = proto_item_add_subtree(awds_item_link, ett_awds_topo_link_lst);
    for(i = 0; i < link_cnt; i++){
      address[0] = tvb_get_guint8(tvb, offset);
      address[1] = tvb_get_guint8(tvb, offset + 1);
      address[2] = tvb_get_guint8(tvb, offset + 2);
      address[3] = tvb_get_guint8(tvb, offset + 3);
      address[4] = tvb_get_guint8(tvb, offset + 4);
      address[5] = tvb_get_guint8(tvb, offset + 5);
      cost = tvb_get_ntohs(tvb, offset + 6);
      awds_item_sublink = proto_tree_add_text(awds_tree_link, tvb, offset, 8, "%02x:%02x:%02x:%02x:%02x:%02x - %d", address[0], address[1], address[2], address[3], address[4], address[5], (int)cost);
      awds_tree_sublink = proto_item_add_subtree(awds_item_sublink, ett_awds_topo_link_entry);     
      proto_tree_add_item(awds_tree_sublink, hf_awds_topo_link_mac_addr, tvb, offset, 6, FALSE);
      proto_tree_add_item(awds_tree_sublink, hf_awds_topo_link_cost, tvb, offset + 6, 2, FALSE);
      offset += 8;
    } //of for
  } //of if

  /* Decode node-name length */
  name_len = tvb_get_guint8(tvb, offset);
  proto_tree_add_item(tree, hf_awds_topo_node_name_len, tvb, offset, 1, FALSE);
  offset++;

  /* Fill information column */
  if(check_col(pinfo->cinfo, COL_INFO))
    col_append_fstr( pinfo->cinfo, COL_INFO, 
		     ", Topology [Node name=\"%s\"])",
		     tvb_get_ephemeral_string( tvb, offset, 
					       (name_len > 32) ? 32 : name_len) );

  /* Decode node-name */
  proto_tree_add_item(tree, hf_awds_topo_node_name, tvb, offset, name_len, FALSE);
  offset += name_len;
} //of dissect_awds_topo

static void dissect_awds_crypto(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, gint offset){
  /* Decode initialisation vector */
  proto_tree_add_item(tree, hf_awds_crypto_iv, tvb, offset, 8, FALSE);
  offset += 8;

  /* Decode nonce value */
  proto_tree_add_item(tree, hf_awds_crypto_nonce, tvb, offset, 8, FALSE);
  offset += 8;

  /* Fill information column */
  if(check_col(pinfo->cinfo, COL_INFO))
    col_append_fstr(pinfo->cinfo, COL_INFO, ", Cryptographic)");
} //of dissect_awds_crypto

static void dissect_awds_data(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, gint offset){
  //gboolean save_info;
  tvbuff_t *next_tvb = NULL;

  /* Fill information column */
  if(check_col(pinfo->cinfo, COL_INFO)){
    col_append_fstr(pinfo->cinfo, COL_INFO, "), ");
    col_set_fence(pinfo->cinfo, COL_INFO);
  } //of if

  /* Call embedded dissector */
  next_tvb = tvb_new_subset(tvb, offset, -1, -1);

  if(eth_handle){
    //save_info = col_get_writable(pinfo->cinfo);
    //col_set_writable(pinfo->cinfo, FALSE);
    call_dissector(eth_handle, next_tvb, pinfo, tree);
    //col_set_writable(pinfo->cinfo, save_info);
  } //of if
} //of dissect_awds_data

static void dissect_awds_flood(tvbuff_t *tvb, 
			       packet_info *pinfo, 
			       proto_tree *tree, 
			       proto_tree *tree_lv1, 
			       gint offset) 
{
  guint8 ttl = 0, subtype = 0;
  proto_item *awds_item = NULL;
  proto_tree *awds_tree = NULL;

  /* Decode Lasthop MAC-address */
  proto_tree_add_item(tree, hf_awds_flood_lasthop_mac_addr, tvb, offset, 6, FALSE);
  offset += 6;

  /* Decode TTL (Time To Live) */
  ttl = tvb_get_guint8(tvb, offset);
  proto_tree_add_item(tree, hf_awds_flood_ttl, tvb, offset, 1, FALSE);
  offset++;

  /* Fill information column */
  if(check_col(pinfo->cinfo, COL_INFO))
    col_add_fstr(pinfo->cinfo, COL_INFO, "Flood (TTL=%d", ttl);

  /* Decode embedded type */
  subtype = tvb_get_guint8(tvb, offset);
  awds_item = proto_tree_add_item(tree, hf_awds_subtype, tvb, offset, 1, FALSE);
  awds_tree = proto_item_add_subtree(awds_item, ett_awds_subtype);
  offset++;

  switch(subtype){
    case AWDS_TOPO:     dissect_awds_topo(tvb, pinfo, awds_tree, offset);
                        break;
    case AWDS_CRYPTO:   dissect_awds_crypto(tvb, pinfo, awds_tree, offset);
                        break;
    case AWDS_CAP_DATA: dissect_awds_data(tvb, pinfo, tree_lv1, offset);
                        break;
  } //of switch
} //dissect_awds_flood

static void dissect_awds_unicast(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, proto_tree *tree_lv1, gint offset){
  guint8 ttl = 0, subtype = 0;
  proto_item *awds_item = NULL;
  proto_tree *awds_tree = NULL;

  /* Decode destination MAC-address */
  proto_tree_add_item(tree, hf_awds_unicast_dest_mac_addr, tvb, offset, 6, FALSE);
  offset += 6;

  /* Decode next-hop MAC-address */
  proto_tree_add_item(tree, hf_awds_unicast_nexthop_mac_addr, tvb, offset, 6, FALSE);
  offset += 6;

  /* Decode TTL */
  ttl = tvb_get_guint8(tvb, offset);
  proto_tree_add_item(tree, hf_awds_unicast_ttl, tvb, offset, 1, FALSE);
  offset++;

  /* Fill information column */
  if(check_col(pinfo->cinfo, COL_INFO))
    col_add_fstr(pinfo->cinfo, COL_INFO, "Unicast (TTL=%d", ttl);

  /* Decode embedded type */
  subtype = tvb_get_guint8(tvb, offset);
  awds_item = proto_tree_add_item(tree, hf_awds_subtype, tvb, offset, 1, FALSE);
  awds_tree = proto_item_add_subtree(awds_item, ett_awds_subtype);
  offset++;

  switch(subtype){
    case AWDS_TOPO:     dissect_awds_topo(tvb, pinfo, awds_tree, offset);
                        break;
    case AWDS_CRYPTO:   dissect_awds_crypto(tvb, pinfo, awds_tree, offset);
                        break;
    case AWDS_CAP_DATA: dissect_awds_data(tvb, pinfo, tree_lv1, offset);
                        break;
  } //of switch
} //dissect_awds_unicast

void dissect_awds(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree){
  gint offset = 0;
  guint8 type = 0, flags = 0;
  proto_item *awds_item = NULL, *awds_subitem = NULL;
  proto_tree *awds_tree = NULL, *awds_subtree = NULL;

  /* Fill Protocol column */
  //if(check_col(pinfo->cinfo, COL_PROTOCOL))
  //  col_add_str(pinfo->cinfo, COL_PROTOCOL, "AWDS");

  /* Clear Information column */
  //if(check_col(pinfo->cinfo, COL_INFO))
  //  col_clear(pinfo->cinfo, COL_INFO);

  /* Asking for details? */
  if(!tree)return;

  /* For all packet types */
  type = tvb_get_guint8(tvb, offset) & 0x03;
  flags = tvb_get_guint8(tvb, offset) >> 2;
  awds_item = proto_tree_add_item(tree, proto_awds, tvb, offset, -1, FALSE);
  awds_tree = proto_item_add_subtree(awds_item, ett_awds);

  /* Decode packet type */
  proto_tree_add_item(awds_tree, hf_awds_type, tvb, offset, 1, FALSE);

  /* Decode packet flags */
  awds_subitem = proto_tree_add_item(awds_tree, hf_awds_flags, tvb, offset, 1, FALSE);
  awds_subtree = proto_item_add_subtree(awds_subitem, ett_awds_flags);
  proto_tree_add_item(awds_subtree, hf_awds_flag_0, tvb, offset, 1, FALSE);
  proto_tree_add_item(awds_subtree, hf_awds_flag_1, tvb, offset, 1, FALSE);
  proto_tree_add_item(awds_subtree, hf_awds_flag_2, tvb, offset, 1, FALSE);
  proto_tree_add_item(awds_subtree, hf_awds_flag_3, tvb, offset, 1, FALSE);
  proto_tree_add_item(awds_subtree, hf_awds_flag_4, tvb, offset, 1, FALSE);
  proto_tree_add_item(awds_subtree, hf_awds_flag_5, tvb, offset, 1, FALSE);
  offset++;

  /* Decode source MAC-address */
  proto_tree_add_item(awds_tree, hf_awds_src_mac_addr, tvb, offset, 6, FALSE);
  offset += 6;

  /* Decode sequence number */
  proto_tree_add_item(awds_tree, hf_awds_seq_no, tvb, offset, 2, FALSE);
  offset += 2;

  /* Decode individual packet-type */
  switch(type){
    case AWDS_BEACON:  dissect_awds_beacon(tvb, pinfo, awds_tree, offset);
                       break;
    case AWDS_FLOOD:   dissect_awds_flood(tvb, pinfo, awds_tree, tree, offset);
                       break;
    case AWDS_UNICAST: dissect_awds_unicast(tvb, pinfo, awds_tree, tree, offset);
                       break;
  } //of switch

  /* Highest priority for this dissector */
  if(check_col(pinfo->cinfo, COL_PROTOCOL))
    col_add_str(pinfo->cinfo, COL_PROTOCOL, "AWDS");
} //of dissect_awds
