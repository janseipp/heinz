/*
 * negedge.h
 *
 *  Created on: 14-jan-2013
 *      Author: M. El-Kebir
 */

#ifndef NEGEDGE_H
#define NEGEDGE_H

#include <lemon/core.h>
#include <string>
#include <vector>
#include <set>
#include "rule.h"

namespace nina {
namespace mwcs {

template<typename GR,
         typename WGHT = typename GR::template NodeMap<double> >
class NegEdge : public Rule<GR, WGHT>
{
public:
  typedef GR Graph;
  typedef WGHT WeightNodeMap;
  typedef Rule<GR, WGHT> Parent;
  typedef typename Parent::NodeMap NodeMap;
  typedef typename Parent::NodeSet NodeSet;
  typedef typename Parent::NodeSetIt NodeSetIt;
  typedef typename Parent::NodeSetMap NodeSetMap;
  typedef typename Parent::DegreeNodeMap DegreeNodeMap;
  typedef typename Parent::DegreeNodeSetVector DegreeNodeSetVector;
  typedef typename Parent::LabelNodeMap LabelNodeMap;
  typedef typename Parent::ArcLookUpType ArcLookUpType;

  TEMPLATE_GRAPH_TYPEDEFS(Graph);

  using Parent::remove;
  using Parent::merge;

  NegEdge();
  virtual ~NegEdge() {}
  virtual int apply(Graph& g,
                    const NodeSet& rootNodes,
                    const ArcLookUpType& arcLookUp,
                    LabelNodeMap& label,
                    WeightNodeMap& score,
                    IntNodeMap& comp,
                    NodeSetMap& mapToPre,
                    NodeSetMap& preOrigNodes,
                    NodeSetMap& neighbors,
                    int& nNodes,
                    int& nArcs,
                    int& nEdges,
                    int& nComponents,
                    DegreeNodeMap& degree,
                    DegreeNodeSetVector& degreeVector,
                    double& LB);

  virtual std::string name() const { return "NegEdge"; }
};

template<typename GR, typename WGHT>
inline NegEdge<GR, WGHT>::NegEdge()
  : Parent()
{
}

template<typename GR, typename WGHT>
inline int NegEdge<GR, WGHT>::apply(Graph& g,
                                    const NodeSet& rootNodes,
                                    const ArcLookUpType& arcLookUp,
                                    LabelNodeMap& label,
                                    WeightNodeMap& score,
                                    IntNodeMap& comp,
                                    NodeSetMap& mapToPre,
                                    NodeSetMap& preOrigNodes,
                                    NodeSetMap& neighbors,
                                    int& nNodes,
                                    int& nArcs,
                                    int& nEdges,
                                    int& nComponents,
                                    DegreeNodeMap& degree,
                                    DegreeNodeSetVector& degreeVector,
                                    double& LB)
{
  int res = 0;

  for (EdgeIt e(g); e != lemon::INVALID; ++e)
  {
    Node u = g.u(e);
    Node v = g.v(e);

    if (score[u] <= 0 && score[v] <= 0 && degree[u] == 2 && degree[v] == 2)
    {
      // only merge if neither u nor v is a root node
      if (rootNodes.find(u) == rootNodes.end() && rootNodes.find(v) == rootNodes.end())
      {
        res++;
        merge(g, arcLookUp, label, score,
              mapToPre, preOrigNodes, neighbors,
              nNodes, nArcs, nEdges,
              degree, degreeVector, u, v, LB);
      }
    }
  }

  return res;
}

} // namespace mwcs
} // namespace nina

#endif // NEGEDGE_H
