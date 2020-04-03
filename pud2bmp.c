/* pud2bmp
   copyright 2007 Greg Kennedy <kennedy.greg@gmail.com>
   released under the GNU GPL v3 */

#define VERSION 1.0

#include <stdio.h>
#include <stdlib.h>

#define TRUCOLOR 1

//TODO: These palette colors are from Wargus.
// Red (p1) is always at 208-211.
// Blue (p2) is nonstandard and is 212-215 in Ice,Swamp, Xswamp, not
//           present at all in Forest.
// p3 is not present in any palette. (?)
// TODO: need p4-p6...
// White (p7) is always at 232-235.
// Yellow (p8) is always at 13-16.
// They only work in true-color mode: we need to find free entries in the
// 256color palette and stuff these in.

// [tileset][indextofirst]
unsigned char pcolors[8][4][3] = {
  {{164, 0, 0}, {124, 0, 0}, {92, 4, 0}, {68, 4, 0}},
  {{12, 72, 204}, {4, 40, 160}, {0, 20, 116}, {0, 4, 76}},
  {{44, 180, 148}, {20, 132, 92}, {4, 84, 44}, {0, 40, 12}},
  {{152, 72, 176}, {116, 44, 132}, {80, 24, 88}, {44, 8, 44}},
  {{248, 140, 20}, {200, 96, 16}, {152, 60, 16}, {108, 32, 12}},
  {{40, 40, 60}, {28, 28, 44}, {20, 20, 32}, {12, 12, 20}},
  {{224, 224, 224}, {152, 152, 180}, {84, 84, 128}, {36, 40, 76}},
  {{252, 252, 72}, {228, 204, 40}, {204, 160, 16}, {180, 116, 0}}
};

//TODO: Get a real GRP loader to get the second frame of art/unit/other/shadow.grp ...
//   or, pack this and use bit-ops to draw the shadow.

unsigned char shadowmap[1024] =
{
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEF,0x00,0xEF,0x00,0xEF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

//TODO: these have GOT to be stored somewhere.
int unitxoff[] = {
    20,20, // grunt
    20,20, // peon
    16,16, // catapult
    20,20, // knight
    20,20, // spear
    20,20, // wiz
    20,20, // pally
    12,12, // demosquad
    20,20, // atk peon
    20,20, // ranger
    20,20, // h:alleria, h:teron
    24,20, // h:kurdan, h:dentarg
    20,20, // h:khadgar, h:grom
    20,20, // tanker
    20,20, // transport
    24,28, // destroyer
    28,28, // bship
     0,28, // h:deathwing
     0, 0, // NULL
    20,20, // sub
    24,20, // flying machine
    24,28, // griffon
    20,20, // h:turalyon, TODO: eye of kilrogg
    20,20, // h:danath, h: khorgath
     0,20, // NULL, h:cho'gall
    20,20, // h:lothar, h:gul'dan
    20,20, // h:uther, h:zuljin
     0,12, // NULL, skele
    20     // daemon
};

int unityoff[] = {
    20,20, // grunt
    20,20, // peon
    16,16, // catapult
    20,20, // knight
    20,20, // spear
    20,20, // wiz
    20,20, // pally
    12,12, // demosquad
    20,20, // atk peon
    20,20, // ranger
    20,20, // h:alleria, h:teron
    40,20, // h:kurdan, h:dentarg
    20,20, // h:khadgar, h:grom
    20,20, // tanker
    20,20, // transport
    28,28, // destroyer
    28,28, // bship
     0,40, // NULL, h:deathwing
     0, 0, // NULL
    20,20, // sub
    40,36, // flying machine
    40,40, // griffon
    20,20, // h:turalyon, TODO: eye of kilrogg
    20,20, // h:danath, h: khorgath
     0,20, // NULL, h:cho'gall
    20,20, // h:lothar, h:gul'dan
    20,20, // h:uther, h:zuljin
     0,12, // NULL, skele
    36     // daemon
};

char *basenames[] = {
//0x0
     "human/grunt.grp",
     "orc/grunt.grp",
     "human/peon.grp",
     "orc/peon.grp",
     "human/catapult.grp",
     "orc/catapult.grp",
     "human/knight.grp",
     "orc/knight.grp",
     "human/spear.grp",
     "orc/spear.grp",
     "human/wizard.grp",
     "orc/dknight.grp",
     "human/knight.grp", // paladin
     "orc/knight.grp",   // ogre-mage
     "human/dwarves.grp",
     "orc/goblins.grp",
//0x10
     "human/peon.grp",  // attack peasant
     "orc/peon.grp",    // attack peon
     "human/spear.grp", // ranger
     "orc/spear.grp",   // berserker
     "human/spear.grp",  // hero: alleria
     "orc/dknight.grp",  // hero: teron gorefiend
     "human/griffon.grp",  // hero: kurdan / skyree
     "orc/knight.grp",  // hero: dentarg
     "human/wizard.grp",  // hero: khadgar
     "orc/grunt.grp",  // hero: grom hellscream
     "human/tanker.grp",
     "orc/tanker.grp",
     "human/transp.grp",
     "orc/transp.grp",
     "human/destroy.grp",
     "orc/destroy.grp",
//0x20
     "human/battlshp.grp",
     "orc/battlshp.grp",
     NULL,
     "orc/dragon.grp",  // hero: deathwing
     NULL,
     NULL,
     "human/sub.grp",
     "orc/sub.grp",
     "human/orn.grp",
     "orc/zep.grp",
     "human/griffon.grp",
     "orc/dragon.grp",
     "human/knight.grp", // hero: turalyon
     "orc/eyeofkil.grp",
     "human/grunt.grp", // hero: danath
     "orc/grunt.grp", // hero: khorgath bladefist
//0x30
     NULL,
     "orc/knight.grp", // hero: cho'gall
     "human/knight.grp", // hero: lothar
     "orc/dknight.grp", // hero: gul'dan
     "human/knight.grp", // hero: uther lightbringer
     "orc/spear.grp", // hero: zuljin
     NULL,
     "orc/skeleton.grp",
     "monster/demon.grp",
     "monster/sheep.grp", // critter!
     "human/farm.grp",
     "orc/farm.grp",
     "human/barr.grp",
     "orc/barr.grp",
     "human/church.grp",
     "orc/temple.grp",
     "human/tower.grp",
     "orc/tower.grp",
     "human/stable.grp",
     "orc/ogrecamp.grp",
     "human/invent.grp",
     "orc/invent.grp",
     "human/aviary.grp",
     "orc/roost.grp",
     "human/ship2.grp",
     "orc/ship2.grp",
     "human/thall.grp",
     "orc/thall.grp",
     "human/lmill.grp",
     "orc/lmill.grp",
     "human/found2.grp",
     "orc/found2.grp",
     "human/wtower.grp",
     "orc/dtower.grp",
     "human/black.grp",
     "orc/black.grp",
     "human/ref2.grp",
     "orc/ref2.grp",
     "human/oplat2.grp",
     "orc/oplat2.grp",
     "human/keep.grp",
     "orc/keep.grp",
     "human/castle.grp",
     "orc/blakrock.grp",
     "other/mine.grp",
     "other/patch.grp",
     "human/startloc.grp",
     "orc/startloc.grp",
     "human/towera.grp",
     "orc/towera.grp",
     "human/towerc.grp",
     "orc/towerc.grp",
     "other/vcircle.grp",
     "other/gate.grp",
     "other/rock.grp",
     "other/fwalunit.grp", // not really certain about how walls work.
     "other/fwalunit.grp"
};

char *snownames[] = { // replace names for snow bldg
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
//0x39
     "monster/seal.grp", // critter!
     "human/s_farm.grp",
     "orc/s_farm.grp",
     "human/s_barr.grp",
     "orc/s_barr.grp",
     "human/s_church.grp",
     "orc/s_temple.grp",
     "human/s_tower.grp",
     "orc/s_tower.grp",
     "human/s_stable.grp",
     "orc/s_ogre.grp",
     "human/s_inv.grp",
     "orc/s_inv.grp",
     "human/s_aviary.grp",
     "orc/s_roost.grp",
     "human/s_ship2.grp",
     "orc/s_ship2.grp",
     "human/s_thall.grp",
     "orc/s_thall.grp",
     "human/s_lmill.grp",
     "orc/s_lmill.grp",
     "human/s_found2.grp",
     "orc/s_found2.grp",
     "human/s_wtower.grp",
     "orc/s_dtower.grp",
     "human/s_black.grp",
     "orc/s_black.grp",
     "human/s_ref2.grp",
     "orc/s_ref2.grp",
     "human/s_oplat2.grp",
     "orc/s_oplat2.grp",
     "human/s_keep.grp",
     "orc/s_keep.grp",
     "human/s_castle.grp",
     "orc/s_blakrk.grp",
     "other/s_mine.grp",
     NULL,
     NULL,
     NULL,
     "human/s_towera.grp",
     "orc/s_towera.grp",
     "human/s_towerc.grp",
     "orc/s_towerc.grp",
     NULL,
     "other/s_gate.grp",
     "other/s_rock.grp",
     NULL,
     NULL
};

char *wastenames[] = { // wasteland - e.g. l_xyz
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     "human/l_sub.grp",
     "orc/l_sub.grp",
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     "monster/boar.grp", // critter!
     "human/l_farm.grp",
     "orc/l_farm.grp",
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     "human/l_lmill.grp",
     "orc/l_lmill.grp",
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     "human/l_oplat2.grp",
     "orc/l_oplat2.grp",
     NULL,
     NULL,
     NULL,
     NULL,
     "other/l_mine.grp",
     "other/l_patch.grp",
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     "other/l_gate.grp",
     NULL,
     NULL,
     NULL
};
     
char *xswampnames[] = {
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     "human/x_sub.grp",
     "orc/x_sub.grp",
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     "monster/hellhog.grp", // critter!
     "human/x_farm.grp",
     "orc/x_farm.grp",
     "human/x_barr.grp",
     "orc/x_barr.grp",
     "human/x_church.grp",
     "orc/x_temple.grp",
     "human/x_tower.grp",
     "orc/x_tower.grp",
     "human/x_stable.grp",
     "orc/x_ogre.grp",
     "human/x_inv.grp",
     "orc/x_inv.grp",
     "human/x_aviary.grp",
     "orc/x_roost.grp",
     "human/x_ship2.grp",
     "orc/x_ship2.grp",
     "human/x_thall.grp",
     "orc/x_thall.grp",
     "human/x_lmill.grp",
     "orc/x_lmill.grp",
     "human/x_found2.grp",
     "orc/x_found2.grp",
     "human/x_wtower.grp",
     "orc/x_dtower.grp",
     "human/x_black.grp",
     "orc/x_black.grp",
     "human/x_ref2.grp",
     "orc/x_ref2.grp",
     "human/x_oplat2.grp",
     "orc/x_oplat2.grp",
     "human/x_keep.grp",
     "orc/x_keep.grp",
     "human/x_castle.grp",
     "orc/x_blakrk.grp",
     "other/x_mine.grp",
     "other/x_patch.grp",
     NULL,
     NULL,
     "human/x_towera.grp",
     "orc/x_towera.grp",
     "human/x_towerc.grp",
     "orc/x_towerc.grp",
     "other/x_vcir.grp",
     "other/x_gate.grp",
     "other/x_rock.grp",
     NULL,
     NULL
};

struct player
{
  unsigned char controller;
  unsigned char race;
  unsigned short gold;
  unsigned short lumber;
  unsigned short oil;
  unsigned char present;
} players[9];

struct unit
{
  unsigned short x,y;
  unsigned char type,owner;
  unsigned short arg;
} *units;

struct grpframe
{
  unsigned short w,h;
  unsigned char *data;
};

unsigned char setname[10];
unsigned char tileset;
unsigned short mapx, mapy;
unsigned short *tilemap;
unsigned long num_units;

unsigned char numpstarts;
unsigned char drawpstarts;

unsigned char *imagedata;

unsigned char pal[256][3];
unsigned short tilegroups[158][16];
unsigned short *megatiles;
unsigned char *minitiles;

struct grpframe *unitimgs[104];

struct grpframe *loadGRP(unsigned char type) // braindead GRP loader for first frame only
{
  unsigned char buffer[80],loop;
//  unsigned short *lineoffsets;
  unsigned short xoff,yoff,i,j,numlines,w,h;
  signed short xpos,ypos;
  struct grpframe *tmp = NULL;
  FILE *fGRP = NULL;

  strcpy(buffer,"art/unit/");
  if (tileset == 1 && snownames[type] != NULL) strcat(buffer,snownames[type]);
  else if (tileset == 2 && wastenames[type] != NULL) strcat(buffer,wastenames[type]);
  else if (tileset == 3 && xswampnames[type] != NULL) strcat(buffer,xswampnames[type]);
  else strcat(buffer,basenames[type]);

  fGRP = fopen(buffer,"rb");
  if (fGRP == NULL) { fprintf(stderr,"ERROR: Couldn't open %s...\n",buffer); return NULL; }

  tmp = (struct grpframe *)malloc(sizeof(struct grpframe));
//  int debug=0;
//  if (strcmp("art/unit/other/mine.grp",buffer) == 0) debug=1;
//  fprintf(stderr,"opened grp %s\n",buffer);
  fread(buffer,1,6,fGRP);
  tmp->w = buffer[2] + 256 * buffer[3];
  tmp->h = buffer[4] + 256 * buffer[5];
  fread(buffer,1,6,fGRP);
  xoff = buffer[0];
  yoff = buffer[1];
  w = buffer[2];
  h = buffer[3];
  fseek(fGRP,buffer[4] + 256 * buffer[5], SEEK_SET); // skip ahead to frame 1
                                     // other frames would have followed

//if (debug==1)  fprintf(stderr,"w %u h %u (%u %u) xoff %u yoff %u frame1 at %x\n",tmp->w,tmp->h,w,h,xoff,yoff,buffer[4] + 256 * buffer[5]);
  fread(buffer,1,2,fGRP);
// cheap
  numlines = (buffer[0] + 256*buffer[1]-(ftell(fGRP)-2)) / 2;
  fseek(fGRP,buffer[0] + 256*buffer[1]-2,SEEK_CUR);
// proper but nonworking : )
/*  numlines = (buffer[0] + 256*buffer[1]-(ftell(fGRP)-2)) / 2;
  printf("%u lines present\n",numlines);
  lineoffsets = (unsigned short *)malloc((numlines+1)*2);
  lineoffsets[0] = buffer[0] + 256 * buffer[1];
  printf("lo[0]=%x\n",lineoffsets[0]);
  for(i=1;i<numlines;i++)
  {
    fread(buffer,1,2,fGRP);
    lineoffsets[i] = buffer[0] + 256 * buffer[1];
    printf("lo[%u]=%x\n",i,lineoffsets[i]);
  }
  printf("done reading lines.  at %x\n",ftell(fGRP));*/
//end
  tmp->data = (unsigned char*)malloc(tmp->w*tmp->h);
  
  memset(tmp->data,0,tmp->w*tmp->h);

xpos = xoff; ypos = 0;

  for (i=0; i<h; i++)
  {
    ypos = i + yoff; // center?
    while (xpos-xoff < w) {
//          fprintf(stderr,"at x:%u y:%u ",xpos,ypos);
    fread(buffer,1,1,fGRP);
    if (buffer[0] >= 0x80) {
      xpos += (buffer[0] - 0x80);
//          fprintf(stderr,"setting xpos to %d\n",xpos);
    } else if (buffer[0] >= 0x40) {
      loop = buffer[0] - 0x40;
      fread(buffer,1,1,fGRP);
//          fprintf(stderr,"looping %u times writing %u\n",loop,buffer[0]);
      for (j=0; j<loop; j++)
      {
        tmp->data[(tmp->w * ypos) + xpos] = buffer[0];
        xpos++;
      }
    } else {
      loop = buffer[0];
//          fprintf(stderr,"writing %u pixels: ",loop);
      for (j=0; j<loop; j++)
      {
        fread(buffer,1,1,fGRP);
//          fprintf(stderr,"%u ",buffer[0]);
        tmp->data[(tmp->w * ypos) + xpos] = buffer[0];
        xpos++;
      }
//          fprintf(stderr,"\n");
    }
   }
   xpos = xoff;
  }
  fclose(fGRP);
//  fprintf(stderr,"ok\n");
  return tmp;
}

unsigned int drawunit(unsigned short u_x,unsigned short u_y,unsigned char type,unsigned char owner)
{
  long i,j,xpos,ypos,imgindex;
  unsigned char mipixel,ispstart;
  if (unitimgs[type] == NULL) unitimgs[type] = loadGRP(type);
  
  if (type == 0x16 || type == 0x23 || type == 0x28 || type == 0x29 || type == 0x2A || type == 0x2B || type == 0x38) 
  // TODO:  Eye of Kilrogg casts shadow?  Not here...
  {
  for (i=0; i<32; i++)
  {
    for (j=0; j<32; j++)
    {
        mipixel = shadowmap[j+(i*32)];
        xpos = (32 * u_x) + j;
        ypos = (32 * u_y) + i;
//        if (type < 0x39) { xpos -= unitxoff[type]; ypos -= unityoff[type]; }
        if (xpos >= 0 && ypos >= 0 && xpos < 32 * mapx && ypos < 32 * mapy && mipixel != 0) { // nontransparent pixel
#ifdef TRUCOLOR
           imgindex = 3 *    (((32 * mapx) * ypos) + xpos);
           imagedata[imgindex] = pal[mipixel][2];
           imagedata[imgindex + 1] = pal[mipixel][1];
           imagedata[imgindex + 2] = pal[mipixel][0];
#else
           imgindex = ((32 * (signed)mapx) * (signed)ypos) + (signed)xpos;
           imagedata[imgindex] = mipixel;
#endif
        }
    }
  }
  }

  for (i=0; i<unitimgs[type]->h; i++)
  {
    for (j=0; j<unitimgs[type]->w; j++)
    {
        mipixel = unitimgs[type]->data[j+(i*unitimgs[type]->w)];
        xpos = (32 * u_x) + j;
        ypos = (32 * u_y) + i;
        if (type < 0x39) { xpos -= unitxoff[type]; ypos -= unityoff[type]; }
        if (xpos >= 0 && ypos >= 0 && xpos < mapx * 32 && ypos < mapy * 32 && mipixel != 0) { // nontransparent pixel
#ifdef TRUCOLOR
        imgindex = 3 *    (((32 * mapx) * ypos) + xpos);
        if (mipixel < 208 || mipixel > 211) {
           imagedata[imgindex] = pal[mipixel][2];
           imagedata[imgindex + 1] = pal[mipixel][1];
           imagedata[imgindex + 2] = pal[mipixel][0];
        } else {
           imagedata[imgindex] = pcolors[owner][mipixel-208][2];
           imagedata[imgindex + 1] = pcolors[owner][mipixel-208][1];
           imagedata[imgindex + 2] = pcolors[owner][mipixel-208][0];
        }
#else
// TODO: this is wrong, see top!
        if (mipixel >= 208 && mipixel <= 211) mipixel += (4 * owner);
           imgindex = ((32 * (signed)mapx) * (signed)ypos) + (signed)xpos;
           imagedata[imgindex] = mipixel;
#endif
        }
    }
  }
}

unsigned int blit(unsigned int tilex, unsigned int tiley, unsigned short tdat)
{
  unsigned long i,j,s,t,mgindex,miindex,imgindex;
  unsigned char mipixel,horiflip,vertflip;
  
  mgindex = tilegroups[tdat/16][tdat%16]; // get megatile number to use
//  fprintf(stderr,"%u %u %u %u %u\n",tdat,tdat/16,tdat%16,tilegroups[tdat/16][tdat%16],mgindex);

  for (s = 0; s < 4; s++)
  {
    for (t = 0; t < 4; t++)
    {
       horiflip = megatiles[(16*mgindex) + (s * 4 + t)] & 0x01;
       vertflip = megatiles[(16*mgindex) + (s * 4 + t)] & 0x02;
       miindex = megatiles[(16*mgindex) + (s * 4 + t)] >> 2;
//       fprintf(stderr,"%u (16*%u) %u %u\n",miindex,mgindex,s,t);
       for(j=0; j<8; j++)
       {
         for(i=0; i< 8; i++)
         {
//           fprintf(stderr,"ok? %u %u\n",i,j);
            if (!horiflip && !vertflip) mipixel = minitiles[i + (8*j) + (64*miindex)];
            else if (vertflip) mipixel = minitiles[(7-i) + (8*j) + (64*miindex)];
            else if (horiflip) mipixel = minitiles[i + (8*(7-j)) + (64*miindex)];
            else mipixel = minitiles[(7-i) + (8*(7-j)) + (64*miindex)];
//           fprintf(stderr,"ok!\n");
                     //24bpp //row
#ifdef TRUCOLOR
           imgindex = 3 *    (((32 * mapx) * ((32 * tiley) + (8*s) + j)) + ((32 * tilex) + (8*t) + i));
//           imgindex = 3 * (((32*mapx) * (32 * tiley + ((8*s)+j))) + (32 * tilex) + ((8*t)+i));
//           if (imgindex >= 1024*3*mapx*mapy) fprintf(stderr,"oops\n");
           imagedata[imgindex] = pal[mipixel][2];
           imagedata[imgindex + 1] = pal[mipixel][1];
           imagedata[imgindex + 2] = pal[mipixel][0];
#else
           imgindex = (((32 * mapx) * ((32 * tiley) + (8*s) + j)) + ((32 * tilex) + (8*t) + i));
           imagedata[imgindex] = mipixel;
#endif
         }
       }
    }
  }

  return 0;
}

unsigned int loadTileset()
{
  // this is a pain.
  // cv4 is tilegroups
  // ppl is palette
  // vr4 are minitiles
  // vx4 are megatiles
  unsigned long i,j,lSize;
  unsigned char buffer[80];
  FILE *fCV4=NULL, *fPPL=NULL, *fVR4=NULL, *fVX4=NULL;
  switch(tileset)
  {
    case 0:
      strcpy(setname,"forest");
      break;
    case 1:
      strcpy(setname,"iceland");
      break;
    case 2:
      strcpy(setname,"swamp");
      break;
    case 3:
      strcpy(setname,"xswamp");
      break;
    default:
      fprintf(stderr,"ERROR: Invalid tileset %u - damaged PUD?\n",tileset);
      return 1;
  }
  fprintf(stderr,"Info: Map uses tileset %s\n",setname);

  sprintf(buffer,"art/bgs/%s/%s.cv4",setname,setname);
  fCV4 = fopen(buffer,"rb");
  sprintf(buffer,"art/bgs/%s/%s.ppl",setname,setname);
  fPPL = fopen(buffer,"rb");
  sprintf(buffer,"art/bgs/%s/%s.vr4",setname,setname);
  fVR4 = fopen(buffer,"rb");
  sprintf(buffer,"art/bgs/%s/%s.vx4",setname,setname);
  fVX4 = fopen(buffer,"rb");
  
  if (fCV4 == NULL || fPPL == NULL || fVR4 == NULL || fVX4 == NULL)
  {
    fprintf(stderr,"ERROR: Bad file open on tileset %s.\n",setname);
    fclose(fCV4); fclose(fPPL); fclose(fVR4); fclose(fVX4); return 1;
  }

  fread(pal,1,768,fPPL);
  fclose(fPPL);
  for (i=0; i<256; i++)
  {
    pal[i][0] = pal[i][0] << 2;
    pal[i][1] = pal[i][1] << 2;
    pal[i][2] = pal[i][2] << 2;
  }
  
  for (i=0; i<158; i++)
  {
    for (j=0; j<16; j++)
    {
      fread(buffer,1,2,fCV4);
      tilegroups[i][j] = buffer[0] + 256 * buffer[1];
    }
    fseek(fCV4,10,SEEK_CUR); // skip 10 unknown bytes...
  }
  fclose(fCV4);

  // obtain file size:
  fseek (fVX4, 0 , SEEK_END);
  lSize = ftell (fVX4);
  rewind (fVX4);
  
//  printf("There are %d megatiles here.\n",lSize/32);

  megatiles = (unsigned short *) malloc (lSize);// sizeof(unsigned short));
  if (megatiles == NULL) { printf("MEMORY ERROR\n"); return 1; }
  for (i=0; i<lSize / 2; i++)
  {
    fread(buffer,1,2,fVX4);
    megatiles[i] = buffer[0] + 256 * buffer[1];
//    printf("%d",i);
  }
  fclose(fVX4);

  // obtain file size:
  fseek (fVR4, 0 , SEEK_END);
  lSize = ftell (fVR4);
  rewind (fVR4);
  
//  printf("There are %d minitiles here.\n",lSize / 64);
  
  minitiles = (unsigned char *)malloc(lSize);
  if (minitiles == NULL) { printf("MEMORY ERROR\n"); return 1; }
  fread(minitiles,1,lSize,fVR4);
  
  fclose(fVR4);
  return 0;
}

int getGRPFrame()
{
}

int main(int argc, char *argv[])
{
  FILE *fPUD = NULL, *fBMP = NULL;
  unsigned char buffer[80];
  unsigned long length,i,j;
  
  unsigned char filename[80],desc[32];
  
                                           //filesize
  unsigned char bmpheader[] = {0x42, 0x4D, 0,0,0,0, 0,0, 0,0, 0x36,0,0,0,
                               0x28,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
                                           //w      //h                          //datasize

  numpstarts = 0;

  if (argc < 2) {
    printf("pud2bmp %f\nGreg Kennedy 2007\nUSAGE: %s [-x] name.pud\n -x: don't draw player starts",VERSION,argv[0]);
    return 255;
  } else {
    if (strcmp(argv[1],"-x") == 0)
    {
      drawpstarts = 0;
      strcpy(filename,argv[2]);
    } else {
      drawpstarts = 1;
      strcpy(filename,argv[1]);
    }
    fPUD = fopen(filename,"rb");
    if (fPUD == NULL) { fprintf(stderr,"ERROR: Problem opening %s...\n",filename); return 1; }

    sprintf(buffer,"%s.bmp",filename);
    fBMP = fopen(buffer,"wb");
    if (fBMP == NULL) { fprintf(stderr,"ERROR: Problem opening %s...\n",buffer); fclose(fPUD); return 1; }

    while (!feof(fPUD)) {
      fread(buffer,1,4,fPUD);
      fread(&length,4,1,fPUD);
      if (strncmp(buffer,"TYPE",4) == 0) {
        fread(buffer,1,9,fPUD);
        if (strncmp(buffer,"WAR2 MAP",9) != 0) {
          fprintf(stderr,"ERROR: Map type is invalid (%s) - damaged PUD?\n",buffer);
          fclose(fPUD);
          fclose(fBMP);
          return 1;
        }
        fread(buffer,1,7,fPUD);
        fprintf(stderr,"Info: Map type is \"WAR2 MAP\"\nInfo: Map ID is \"%s\"\n",buffer);
      } else if (strncmp(buffer,"VER ",4) == 0) {
        fread(buffer,1,2,fPUD);
        fprintf(stderr,"Info: Map is PUD version $%x\n",buffer[0] + 256 * buffer[1]);
      } else if (strncmp(buffer,"DESC",4) == 0) {
        fread(desc,1,32,fPUD);
        fprintf(stderr,"Info: Map description is \"%s\"\n",desc);
      } else if (strncmp(buffer,"OWNR",4) == 0) {
        for (i = 0; i < 8; i++)
        {
          fread(&players[i].controller,1,1,fPUD);
          players[i].present = 0;
        }
        fseek(fPUD,7,SEEK_CUR);
        fread(&players[8].controller,1,1,fPUD);
      } else if (strncmp(buffer,"ERA ",4) == 0) {
        fread(buffer,1,2,fPUD);
        tileset = buffer[0] + 256 * buffer[1];
        if (tileset > 0x03) tileset = 0;
//        if (loadTileset()) { fclose(fPUD); fclose(fBMP); return 1; }
        fprintf(stderr,"Info: Map tileset is %u\n",tileset);
      } else if (strncmp(buffer,"ERAX",4) == 0) {
        fread(buffer,1,2,fPUD);
        tileset = buffer[0] + 256 * buffer[1];
        if (tileset > 0x03) tileset = 0;
        fprintf(stderr,"Info: Map tilesetX is %u\n",tileset);
      } else if (strncmp(buffer,"DIM ",4) == 0) {
        fread(buffer,1,2,fPUD);
        mapx = buffer[0] + 256 * buffer[1];
        fread(buffer,1,2,fPUD);
        mapy = buffer[0] + 256 * buffer[1];
        tilemap = (unsigned short *) malloc (mapy*mapx * sizeof(unsigned short));
        if (tilemap == NULL) { fprintf(stderr,"ERROR: out of memory allocating tilemap.\n"); return 1; }
        fprintf(stderr,"Info: Map dimensions are %u x %u\n",mapx,mapy);
      } else if (strncmp(buffer,"UDTA",4) == 0) {
        fread(buffer,1,2,fPUD);
        if (buffer[0] == 0)
          fprintf(stderr,"Info: Map contains custom unit data.\n");
        else
          fprintf(stderr,"Info: Map uses default unit data.\n");
        fseek(fPUD,length-2,SEEK_CUR);
      } else if (strncmp(buffer,"ALOW",4) == 0) {
        //don't care much about this now, although it may be interesting
        // to note in map info
        fseek(fPUD,length,SEEK_CUR);
      } else if (strncmp(buffer,"UGRD",4) == 0) {
        fread(buffer,1,2,fPUD);
        if (buffer[0] == 0)
          fprintf(stderr,"Info: Map contains custom upgrade data.\n");
        else
          fprintf(stderr,"Info: Map uses default upgrade data.\n");
        fseek(fPUD,length-2,SEEK_CUR);
      } else if (strncmp(buffer,"SIDE",4) == 0) {
        for (i = 0; i < 8; i++)
          fread(&players[i].race,1,1,fPUD);
        fseek(fPUD,7,SEEK_CUR);
        fread(&players[8].race,1,1,fPUD);
      } else if (strncmp(buffer,"SGLD",4) == 0) {
        for (i = 0; i < 8; i++)
        {
          fread(buffer,1,2,fPUD);
          players[i].gold = buffer[0] + 256 * buffer[1];
        }
        fseek(fPUD,14,SEEK_CUR);
        fread(buffer,1,2,fPUD);
        players[8].gold = buffer[0] + 256 * buffer[1];
      } else if (strncmp(buffer,"SLBR",4) == 0) {
        for (i = 0; i < 8; i++)
        {
          fread(buffer,1,2,fPUD);
          players[i].lumber = buffer[0] + 256 * buffer[1];
        }
        fseek(fPUD,14,SEEK_CUR);
        fread(buffer,1,2,fPUD);
        players[8].lumber = buffer[0] + 256 * buffer[1];
      } else if (strncmp(buffer,"SOIL",4) == 0) {
        for (i = 0; i < 8; i++)
        {
          fread(buffer,1,2,fPUD);
          players[i].oil = buffer[0] + 256 * buffer[1];
        }
        fseek(fPUD,14,SEEK_CUR);
        fread(buffer,1,2,fPUD);
        players[8].oil = buffer[0] + 256 * buffer[1];
      } else if (strncmp(buffer,"AIPL",4) == 0) {
        // Don't care much about AI info
        fseek(fPUD,length,SEEK_CUR);
      } else if (strncmp(buffer,"MTXM",4) == 0) {
//        fread(tilemap,1,mapx*mapy*2,fPUD);
          for (i=0; i<mapx*mapy; i++) {
            fread(buffer,1,2,fPUD);
            tilemap[i] = buffer[0] + 256 * buffer[1];
          }        
      } else if (strncmp(buffer,"SQM ",4) == 0) {
        // Don't care much about movement map info
        fseek(fPUD,length,SEEK_CUR);
      } else if (strncmp(buffer,"OILM",4) == 0) {
        // Oil map info is completely unused
        fseek(fPUD,length,SEEK_CUR);
      } else if (strncmp(buffer,"REGM",4) == 0) {
        // Don't care much about action map info
        fseek(fPUD,length,SEEK_CUR);
      } else if (strncmp(buffer,"UNIT",4) == 0) {
        num_units = length / 8;
        units = (struct unit*)malloc(sizeof(struct unit) * num_units);
        fprintf(stderr,"Info: Map contains %u units.\n",num_units);
        for (i=0; i<num_units; i++) {
          fread(buffer,1,2,fPUD);
          units[i].x = buffer[0] + 256 * buffer[1];
          fread(buffer,1,2,fPUD);
          units[i].y = buffer[0] + 256 * buffer[1];
          fread(&units[i].type,1,1,fPUD);
          fread(&units[i].owner,1,1,fPUD);
          fread(buffer,1,2,fPUD);
          units[i].arg = buffer[0] + 256 * buffer[1];
        }
      } else if (strncmp(buffer,"SIGN",4) == 0) {
        fread(buffer,1,4,fPUD);
        fprintf(stderr,"Info: Map has official signature %08x\n",*buffer);
      } else {
        if (!feof(fPUD)) {
          fprintf(stderr,"ERROR: Unknown section encountered - damaged PUD?\n");
//          fseek(fPUD,length,SEEK_CUR);
          // we used to exit here, but evidently PUD has multiple revisions...
          // just try seeking past it here.
          fclose(fPUD);
          fclose(fBMP);
          return 1;
        }
      }
    }

    if (loadTileset()) { fclose(fPUD); fclose(fBMP); return 1; }

#ifdef TRUCOLOR
    imagedata = (unsigned char*)malloc(3 * 1024 * mapx * mapy * sizeof(unsigned char));
#else
    imagedata = (unsigned char*)malloc(1024 * mapx * mapy * sizeof(unsigned char));
#endif

    // mangle BMP header
    memcpy(buffer,bmpheader,54);
#ifdef TRUCOLOR
    unsigned int temp=(unsigned int)(54 + (3 * 1024 * mapx * mapy));
#else
    buffer[11] = 4;
    buffer[28] = 8; // 8bpp
    buffer[48] = 1;
    buffer[52] = 1;
    unsigned int temp=(unsigned int)(1078 + (1024 * mapx * mapy));
#endif
    memcpy(&buffer[2],&temp,4);
    temp = (unsigned int)(32*mapx);
    memcpy(&buffer[18],&temp,4);
    temp = (unsigned int)(32*mapy);
    memcpy(&buffer[22],&temp,4);
#ifdef TRUCOLOR
    temp = 3 * 1024 * mapx * mapy; // size without header
#else
    temp = 1024 * mapx * mapy;
#endif
    memcpy(&buffer[34],&temp,4);
    fwrite(buffer,1,54,fBMP);
#ifndef TRUCOLOR
    for (i=0; i<256; i++)
    {
      fwrite(&pal[i][2],1,1,fBMP);
      fwrite(&pal[i][1],1,1,fBMP);
      fwrite(&pal[i][0],1,1,fBMP);
      fwrite("",1,1,fBMP);
    }
#endif
    
    for (j=0; j<mapy; j++)
      for (i=0; i<mapx; i++)
        blit(i,j,tilemap[(i+(j*mapx))]);

    for (i=0; i<num_units; i++)
      if (units[i].type == 0x5E || units[i].type == 0x5F)
      {
        numpstarts++;
        players[units[i].owner].present = 1;
        if (drawpstarts) drawunit(units[i].x,units[i].y,units[i].type,units[i].owner);
      } else {
        drawunit(units[i].x,units[i].y,units[i].type,units[i].owner);
      }
  
    // turn image right-side-up
    unsigned char* flipbuf;
#ifdef TRUCOLOR
    flipbuf = (unsigned char*)malloc(96*mapx);

    for (i=0; i<16*mapy; i++)
    {
      memcpy(flipbuf,&imagedata[i*96*mapx],96*mapx);
      memcpy(&imagedata[i*96*mapx],&imagedata[(32*mapy-1-i)*96*mapx],96*mapx);
      memcpy(&imagedata[(32*mapy-1-i)*96*mapx],flipbuf,96*mapx);
    }
#else
    flipbuf = (unsigned char*)malloc(32*mapx);

    for (i=0; i<16*mapy; i++)
    {
      memcpy(flipbuf,&imagedata[i*32*mapx],32*mapx);
      memcpy(&imagedata[i*32*mapx],&imagedata[(32*mapy-1-i)*32*mapx],32*mapx);
      memcpy(&imagedata[(32*mapy-1-i)*32*mapx],flipbuf,32*mapx);
    }
#endif    
    free(flipbuf);
    // end cheap flip

    fwrite(imagedata,1,temp, fBMP);

    free(imagedata);

    free(megatiles);
    free(minitiles);

    free(units);
    free(tilemap);

    for (i=0; i<104; i++)
    {
      if (unitimgs[i] != NULL) {
        free(unitimgs[i]->data);
        free(unitimgs[i]);
      }
    }

    fclose(fBMP);
    fclose(fPUD);
    
    // produce report
    int numplayers = 0;
    for (i=0; i<8; i++)
      if (players[i].controller == 0x05) numplayers++;

    if (tileset == 0x01) strcpy(setname,"Winter");
    else if (tileset == 0x02) strcpy(setname,"Wasteland");
    else if (tileset == 0x03) strcpy(setname,"Draenor");
    else strcpy(setname,"Forest");

    printf("Filename:    %s\nDescription: %s\nDimensions:  %u x %u\nTerrain:     %s\nPlayers:     %u\n\n",filename,desc,mapx,mapy,setname,numplayers,players[0].gold,players[0].lumber,players[0].oil);

    char *colors[] = {"Red","Blue","Green","Purple","Orange","Black","White","Yellow"};
    char *controllers[] = {"Computer","Computer","Computer","Nobody","Computer","Person","Rescue","Rescue"};
    
    for (i=0; i<8; i++)
    {
        if (players[i].present) printf("Player %u (%s) %s - %s\n  (G=%u  L=%u  O=%u)\n\n",i+1,colors[i],controllers[players[i].controller % 0x08],(players[i].race==0x00?"Human":"Orc"),players[i].gold,players[i].lumber,players[i].oil);
    }
    return 0;
  }
}
