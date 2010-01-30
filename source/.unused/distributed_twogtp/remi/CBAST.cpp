/////////////////////////////////////////////////////////////////////////////
//
// CBAST.cpp
//
// Rémi Coulom
//
// April, 2009
//
/////////////////////////////////////////////////////////////////////////////
#include "CBAST.h"

#include <cmath>
#include <cassert>

/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
CBAST::CBAST(int DimensionsInit, double ExplorationInit):
 area(DimensionsInit),
 Dimensions(DimensionsInit),
 Exploration(ExplorationInit),
 pnodeRoot(0)
{
 OnReset();
}

/////////////////////////////////////////////////////////////////////////////
// NextSample
/////////////////////////////////////////////////////////////////////////////
const std::vector<double> &CBAST::NextSample(int i)
{
 CNode *pnode = pnodeRoot;
 area.Reset();

 while (true)
 {
  if (pnode->outcome == CResults::Unknown)
  {
   pnode->outcome = CResults::InProgress;
   mInProgress[i] = pnode;
   const std::vector<double> &v = area.vCenter;

   do
   {
    pnode->Games++;
    pnode = pnode->pnodeParent;
   }
   while (pnode);

   return v;
  }

  if (!pnode->tpnodeChild[0])
  {
   pnode->CreateChildren();
   pnode = area.Follow(*pnode, 1);
   continue;
  }

  double LogT = std::log(double(pnode->Games)) * 0.5;

  double v0 = pnode->tpnodeChild[0]->Victories + 1;
  double n0 = pnode->tpnodeChild[0]->Games + 1;
  double v1 = pnode->tpnodeChild[1]->Victories + 1;
  double n1 = pnode->tpnodeChild[1]->Games + 1;

  if (n1 * (v0 + Exploration * std::sqrt(LogT * n0)) >
      n0 * (v1 + Exploration * std::sqrt(LogT * n1)))
   pnode = area.Follow(*pnode, 0);
  else
   pnode = area.Follow(*pnode, 1);
 }
}

/////////////////////////////////////////////////////////////////////////////
// MaxParameter
/////////////////////////////////////////////////////////////////////////////
bool CBAST::MaxParameter(std::vector<double> &vMax) const
{
 CNode *pnode = pnodeRoot;
 area.Reset();

 while (pnode->Victories &&
        pnode->tpnodeChild[0] &&
        pnode->tpnodeChild[0]->Games &&
        pnode->tpnodeChild[1]->Games &&
        (pnode->tpnodeChild[0]->Victories + pnode->tpnodeChild[1]->Victories))
 {
  int v0 = pnode->tpnodeChild[0]->Victories;
  int n0 = pnode->tpnodeChild[0]->Games;
  int v1 = pnode->tpnodeChild[1]->Victories;
  int n1 = pnode->tpnodeChild[1]->Games;

  if (v0 * n1 > v1 * n0)
   pnode = area.Follow(*pnode, 0);
  else
   pnode = area.Follow(*pnode, 1);
 }

 vMax = area.vCenter;

 return true;
}

/////////////////////////////////////////////////////////////////////////////
// OnOutcome
/////////////////////////////////////////////////////////////////////////////
void CBAST::OnOutcome(CResults::Outcome outcome, int i)
{
 std::map<int, CNode *>::iterator mi = mInProgress.find(i);
 assert(mi != mInProgress.end());
 CNode *pnode = mi->second;
 assert(pnode->outcome == CResults::InProgress);
 mInProgress.erase(mi);

 pnode->outcome = outcome;

 if (outcome == CResults::Win)
  do
  {
   pnode->Victories++;
   pnode = pnode->pnodeParent;
  }
  while (pnode);
}

/////////////////////////////////////////////////////////////////////////////
// OnReset()
/////////////////////////////////////////////////////////////////////////////
void CBAST::OnReset()
{
 if (pnodeRoot)
  delete pnodeRoot;

 pnodeRoot = new CNode();
 pnodeRoot->Depth = 0;
 pnodeRoot->pnodeParent = 0;
 pnodeRoot->outcome = CResults::InProgress;

 mInProgress.clear();
}

/////////////////////////////////////////////////////////////////////////////
// CNode constructor
/////////////////////////////////////////////////////////////////////////////
CBAST::CNode::CNode():
 outcome(CResults::Unknown),
 Games(0),
 Victories(0)
{
 tpnodeChild[0] = 0;
 tpnodeChild[1] = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CNode destructor
/////////////////////////////////////////////////////////////////////////////
CBAST::CNode::~CNode()
{
 for (int i = 2; --i >= 0;)
  if (tpnodeChild[i])
   delete tpnodeChild[i];
}

/////////////////////////////////////////////////////////////////////////////
// Create node children
/////////////////////////////////////////////////////////////////////////////
void CBAST::CNode::CreateChildren()
{
 for (int i = 2; --i >= 0;)
 {
  CNode &node = *new CNode();
  tpnodeChild[i] = &node;
  node.pnodeParent = this;
  node.Depth = Depth + 1;
 }
}
