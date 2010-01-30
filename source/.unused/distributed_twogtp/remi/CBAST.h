/////////////////////////////////////////////////////////////////////////////
//
// CBAST.h
//
// Rémi Coulom
//
// April, 2009
//
/////////////////////////////////////////////////////////////////////////////
#ifndef CBAST_Declared
#define CBAST_Declared

#include <map>
#include <vector>

class CResults // results
{
 public: ////////////////////////////////////////////////////////////////////
  enum Outcome {Loss = 0, Win = 1, Unknown = 2, InProgress = 3};
};

class CBAST
{
 private: ///////////////////////////////////////////////////////////////////
  struct CNode // node
  {
   CNode *tpnodeChild[2];
   CNode *pnodeParent;
   int Depth;

   CResults::Outcome outcome;

   int Games;
   int Victories;

   CNode();
   ~CNode();

   void CreateChildren();
  };

 private: ///////////////////////////////////////////////////////////////////
  mutable struct CArea
  {
   std::vector<double> vCenter;
   std::vector<double> vRadius;

   CArea(int n): vCenter(n), vRadius(n) {Reset();}

   void Reset()
   {
    for (int i = vCenter.size(); --i >= 0;)
    {
     vCenter[i] = 0.0;
     vRadius[i] = 1.0;
    }
   }

   CNode *Follow(const CNode &node, int i)
   {
    const int Dim = node.Depth % vCenter.size();
    vRadius[Dim] *= 0.5;
    vCenter[Dim] += vRadius[Dim] * (2 * i - 1);
    return node.tpnodeChild[i];
   }
  } area;

 private: ///////////////////////////////////////////////////////////////////
  const int Dimensions;
  const double Exploration;

  CNode *pnodeRoot;
  std::map<int, CNode *> mInProgress;

 public: ////////////////////////////////////////////////////////////////////
  CBAST(int DimensionsInit, double ExplorationInit = 1.0);

  const std::vector<double> &NextSample(int i);
  bool MaxParameter(std::vector<double> &vMax) const;
  void OnOutcome(CResults::Outcome outcome, int i);
  void OnReset();
};

#endif
