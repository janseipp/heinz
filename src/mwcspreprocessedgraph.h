/*
 * mwcspreprocessedgraph.h
 *
 *  Created on: 11-jan-2013
 *      Author: M. El-Kebir
 */

#ifndef MWCSPREPROCESSEDGRAPH_H
#define MWCSPREPROCESSEDGRAPH_H

#include "mwcsgraphparser.h"
#include "preprocessing/rule.h"
#include <set>
#include <vector>
#include <algorithm>
#include <lemon/core.h>

#include "preprocessing/negdeg01.h"
#include "preprocessing/posedge.h"
#include "preprocessing/negedge.h"
#include "preprocessing/negcircuit.h"
#include "preprocessing/negdiamond.h"
#include "preprocessing/negmirroredhubs.h"
#include "preprocessing/posdeg01.h"
#include "preprocessing/posdiamond.h"
#include "preprocessing/negbicomponent.h"
#include "preprocessing/negtricomponent.h"

namespace nina {
namespace mwcs {

template<typename GR,
         typename NWGHT = typename GR::template NodeMap<double>,
         typename NLBL = typename GR::template NodeMap<std::string>,
         typename EWGHT = typename GR::template EdgeMap<double> >
class MwcsPreprocessedGraph : public MwcsGraphParser<GR, NWGHT, NLBL, EWGHT>
{
public:
  typedef GR Graph;
  typedef NWGHT WeightNodeMap;
  typedef NLBL LabelNodeMap;
  typedef EWGHT WeightEdgeMap;

  typedef MwcsGraphParser<GR, NWGHT, NLBL, EWGHT> Parent;
  typedef Rule<GR, NWGHT> RuleType;
  typedef typename Parent::ParserType ParserType;
  typedef typename Parent::InvLabelNodeMap InvLabelNodeMap;
  typedef typename Parent::InvLabelNodeMapIt InvLabelNodeMapIt;
  typedef typename RuleType::DegreeNodeMap DegreeNodeMap;
  typedef typename RuleType::DegreeNodeSetVector DegreeNodeSetVector;
  typedef typename RuleType::NodeMap NodeMap;
  typedef typename RuleType::NodeSet NodeSet;
  typedef typename RuleType::NodeSetIt NodeSetIt;
  typedef typename RuleType::NodeSetMap NodeSetMap;
  
  TEMPLATE_GRAPH_TYPEDEFS(Graph);
  
  typedef std::set<Edge> EdgeSet;
  typedef typename EdgeSet::const_iterator EdgeSetIt;

  using Parent::getGraph;
  using Parent::getScores;
  using Parent::getOrgArcCount;
  using Parent::getOrgArcLookUp;
  using Parent::getOrgComponent;
  using Parent::getOrgComponentCount;
  using Parent::getOrgComponentMap;
  using Parent::getOrgEdgeCount;
  using Parent::getOrgGraph;
  using Parent::getOrgLabel;
  using Parent::getOrgLabels;
  using Parent::getOrgNodeByLabel;
  using Parent::getOrgNodeCount;
  using Parent::getOrgScore;
  using Parent::getOrgScores;
  using Parent::_parserInit;

private:
  typedef std::vector<RuleType*> RuleVector;
  typedef typename RuleVector::const_iterator RuleVectorIt;
  typedef typename RuleVector::iterator RuleVectorNonConstIt;
  typedef std::vector<RuleVector> RuleMatrix;

public:
  MwcsPreprocessedGraph();
  virtual ~MwcsPreprocessedGraph();
  virtual bool init(ParserType* pParser, bool pval);
  void preprocess(const NodeSet& rootNodes);

protected:
  typedef typename Parent::ArcLookUpType ArcLookUpType;
  
  typedef NegDeg01<Graph> NegDeg01Type;
  typedef PosEdge<Graph> PosEdgeType;
  typedef NegEdge<Graph> NegEdgeType;
  typedef NegCircuit<Graph> NegCircuitType;
  typedef NegDiamond<Graph> NegDiamondType;
  typedef NegMirroredHubs<Graph> NegMirroredHubsType;
  typedef PosDeg01<Graph> PosDeg01Type;
  typedef PosDiamond<Graph> PosDiamondType;
  typedef NegBiComponent<Graph> NegBiComponentType;
  typedef NegTriComponent<Graph> NegTriComponentType;

private:
  typedef struct GraphStruct
  {
    Graph* _pG;
    LabelNodeMap* _pLabel;
    WeightNodeMap* _pScore;
    IntNodeMap* _pComp;
    NodeSetMap* _pPreOrigNodes;
    NodeSetMap* _pMapToPre;
    int _nNodes;
    int _nEdges;
    int _nArcs;
    int _nComponents;
    ArcLookUpType* _pArcLookUp;

    // constructor
    GraphStruct(const Graph& orgG)
      : _pG(new Graph())
      , _pLabel(new LabelNodeMap(*_pG))
      , _pScore(new WeightNodeMap(*_pG))
      , _pComp(new IntNodeMap(*_pG))
      , _pPreOrigNodes(new NodeSetMap(*_pG))
      , _pMapToPre(new NodeSetMap(orgG))
      , _nNodes(0)
      , _nEdges(0)
      , _nArcs(0)
      , _nComponents(0)
      , _pArcLookUp(new ArcLookUpType(*_pG))
    {
    }

    // destructor
    ~GraphStruct()
    {
      delete _pArcLookUp;
      delete _pPreOrigNodes;
      delete _pMapToPre;
      delete _pComp;
      delete _pScore;
      delete _pLabel;
      delete _pG;
    }
  } GraphStruct;

private:
  GraphStruct* _pGraph;
  GraphStruct* _pBackupGraph;
  RuleMatrix _rules;

protected:
  virtual void initParserMembers(Graph*& pG,
                                 LabelNodeMap*& pLabel,
                                 WeightNodeMap*& pScore,
                                 WeightNodeMap*& pPVal)
  {
    Parent::initParserMembers(pG, pLabel, pScore, pPVal);
    _pGraph = new GraphStruct(*pG);
  }

  virtual const ArcLookUpType& getArcLookUp() const { return *_pGraph->_pArcLookUp; }

public:
  virtual const Graph& getGraph() const
  {
    return *_pGraph->_pG;
  }

  virtual Graph& getGraph()
  {
    return *_pGraph->_pG;
  }

  virtual NodeSet getOrgNodes(Node node) const
  {
    return (*_pGraph->_pPreOrigNodes)[node];
  }

  virtual NodeSet getOrgNodes(const NodeSet& nodes) const
  {
    NodeSet result;
    const NodeSetMap& preOrigNodes = *_pGraph->_pPreOrigNodes;

    for (NodeSetIt nodeIt = nodes.begin(); nodeIt != nodes.end(); nodeIt++)
    {
      result.insert(preOrigNodes[*nodeIt].begin(), preOrigNodes[*nodeIt].end());
    }

    return result;
  }

  virtual const LabelNodeMap& getLabels() const
  {
    return *_pGraph->_pLabel;
  }

  virtual LabelNodeMap& getLabels()
  {
    return *_pGraph->_pLabel;
  }

  virtual const WeightNodeMap& getScores() const
  {
    return *_pGraph->_pScore;
  }

  virtual WeightNodeMap& getScores()
  {
    return *_pGraph->_pScore;
  }

  virtual int getNodeCount() const
  {
    return _pGraph->_nNodes;
  }

  virtual int getEdgeCount() const
  {
    return _pGraph->_nEdges;
  }

  virtual int getArcCount() const
  {
    return _pGraph->_nArcs;
  }

  virtual int getComponentCount() const
  {
    return _pGraph->_nComponents;
  }

  virtual int getComponent(Node n) const
  {
    assert(n != lemon::INVALID);
    return (*_pGraph->_pComp)[n];
  }

  virtual const IntNodeMap& getComponentMap() const
  {
    return *_pGraph->_pComp;
  }

  virtual double getScore(Node n) const
  {
    assert(n != lemon::INVALID);
    return (*_pGraph->_pScore)[n];
  }

  virtual NodeSet getNodeByLabel(const std::string& label) const
  {
    Node orgNode = getOrgNodeByLabel(label);
    if (orgNode != lemon::INVALID)
    {
      return (*_pGraph->_pMapToPre)[orgNode];
    }
    
    return NodeSet();
  }

  void addPreprocessRule(int phase, RuleType* pRule)
  {
    while (static_cast<int>(_rules.size()) < phase)
      _rules.push_back(RuleVector());
    
    _rules[phase - 1].push_back(pRule);
  }

  virtual std::string getLabel(Node n) const
  {
    assert(n != lemon::INVALID);
    return (*_pGraph->_pLabel)[n];
  }

  virtual bool init(Graph* pG,
                    LabelNodeMap* pLabel,
                    WeightNodeMap* pScore,
                    WeightNodeMap* pPVal);

  virtual void computeScores(double lambda, double a, double FDR);

  virtual void computeScores(double tau);

  virtual NodeSet getPreNodes(Node orgNode) const
  {
    assert(_pGraph->_pMapToPre);
    return (*_pGraph->_pMapToPre)[orgNode];
  }
  
  void clear()
  {
    if (!_pGraph)
    {
      _pGraph = new GraphStruct(getOrgGraph());
    }
    
    _pGraph->_nNodes = getOrgNodeCount();
    _pGraph->_nEdges = getOrgEdgeCount();
    _pGraph->_nArcs = getOrgArcCount();
    _pGraph->_nComponents = getOrgComponentCount();
    
    NodeMap nodeRef(getOrgGraph());

    lemon::graphCopy(getOrgGraph(), *_pGraph->_pG)
        .nodeMap(getOrgScores(), *_pGraph->_pScore)
        .nodeMap(getOrgLabels(), *_pGraph->_pLabel)
        .nodeMap(getOrgComponentMap(), *_pGraph->_pComp)
        .nodeRef(nodeRef)
        .run();

    for (NodeIt n(getOrgGraph()); n != lemon::INVALID; ++n)
    {
      Node preNode = nodeRef[n];
      (*_pGraph->_pPreOrigNodes)[preNode].clear();
      (*_pGraph->_pPreOrigNodes)[preNode].insert(n);
      (*_pGraph->_pMapToPre)[n].clear();
      (*_pGraph->_pMapToPre)[n].insert(preNode);
    }
  }
  
  void remove(Node node)
  {
    Graph& g = *_pGraph->_pG;
    
    // update edge and arc counts
    bool isolated = true;
    for (IncEdgeIt e(g, node); e != lemon::INVALID; ++e)
    {
      _pGraph->_nEdges--;
      _pGraph->_nArcs -= 2;
      isolated = false;
    }
    
    // update mapToPre
    const NodeSet& nodes = (*_pGraph->_pPreOrigNodes)[node];
    for (NodeSetIt nodeIt = nodes.begin(); nodeIt != nodes.end(); nodeIt++)
    {
      // unmap
      (*_pGraph->_pMapToPre)[*nodeIt].erase(node);
    }
    
    // remove the node from the graph
    if (isolated)
    {
      --_pGraph->_nComponents;
    }
    
    g.erase(node);
    --_pGraph->_nNodes;
  }
  
  void remove(const NodeSet& nodes)
  {
    for (NodeSetIt nodeIt = nodes.begin(); nodeIt != nodes.end(); ++nodeIt)
    {
      remove(*nodeIt);
    }
  }
  
  Node merge(Edge e)
  {
    Graph& g = *_pGraph->_pG;
    
    Node u = g.u(e);
    Node v = g.v(e);
    
    // rewire the edges incident to u to v
    for (IncEdgeIt ee(g, u); ee != lemon::INVALID;)
    {
      Node node = g.oppositeNode(u, ee);

      if (node != v)
      {
        if ((*_pGraph->_pArcLookUp)(v, node) == lemon::INVALID)
        {
          // introduce new edge between v and node
          g.addEdge(v, node);
        }
        else
        {
          --_pGraph->_nEdges;
          _pGraph->_nArcs -= 2;
        }
        
        // remove edge between minNode and node
        Edge toDelete = ee;
        ++ee;
        g.erase(toDelete);
      }
      else
      {
        ++ee;
      }
    }
    
    // update score of v
    (*_pGraph->_pScore)[v] += (*_pGraph->_pScore)[u];
    
    // update set of original nodes corresponding to v
    const NodeSet& uOrgNodeSet = (*_pGraph->_pPreOrigNodes)[u];
    for (NodeSetIt nodeIt = uOrgNodeSet.begin(); nodeIt != uOrgNodeSet.end(); nodeIt++)
    {
      (*_pGraph->_pPreOrigNodes)[v].insert(*nodeIt);
      (*_pGraph->_pMapToPre)[*nodeIt].erase(*nodeIt);
      (*_pGraph->_pMapToPre)[*nodeIt].insert(v);
    }
    
    // merge the labels
    (*_pGraph->_pLabel)[v] += "\t" + (*_pGraph->_pLabel)[u];
    
    // erase minNode
    g.erase(u);
    --_pGraph->_nNodes;
    --_pGraph->_nEdges;
    _pGraph->_nArcs -= 2;
    
    return v;
  }
  
  Node merge(NodeSet nodes)
  {
    const Graph& g = *_pGraph->_pG;
    
    // determine edge set
    EdgeSet edges;
    for (EdgeIt e(g); e != lemon::INVALID; ++e)
    {
      if (nodes.find(g.u(e)) != nodes.end() && nodes.find(g.v(e)) != nodes.end())
      {
        edges.insert(e);
      }
    }
    
    Node res;
    for (EdgeSetIt edgeIt = edges.begin(); edgeIt != edges.end(); ++edgeIt)
    {
      res = merge(*edgeIt);
    }
    
    return res;
  }
  
  Node extract(NodeSet nodes)
  {
    Graph& g = *_pGraph->_pG;
    Node res = g.addNode();
    
    (*_pGraph->_pScore)[res] = 0;
    
    for (NodeSetIt nodeIt = nodes.begin(); nodeIt != nodes.end(); ++nodeIt)
    {
      Node v = *nodeIt;
      const NodeSet& orgNodes = (*_pGraph->_pPreOrigNodes)[v];
      
      (*_pGraph->_pScore)[res] += (*_pGraph->_pScore)[v];
      (*_pGraph->_pLabel)[res] += "\t" + (*_pGraph->_pLabel)[v];
      (*_pGraph->_pPreOrigNodes)[res].insert(orgNodes.begin(), orgNodes.end());
      
      for (NodeSetIt orgNodeIt = orgNodes.begin(); orgNodeIt != orgNodes.end(); ++orgNodeIt)
      {
        (*_pGraph->_pMapToPre)[*orgNodeIt].insert(res);
      }
    }
    
    return res;
  }

protected:
  void constructDegreeMap(DegreeNodeMap& degree,
                          DegreeNodeSetVector& degreeVector) const;
  void constructNeighborMap(NodeSetMap& neighbors) const;
};

template<typename GR, typename NWGHT, typename NLBL, typename EWGHT>
inline MwcsPreprocessedGraph<GR, NWGHT, NLBL, EWGHT>::MwcsPreprocessedGraph()
  : Parent()
  , _pGraph(NULL)
  , _pBackupGraph(NULL)
  , _rules()
{
  addPreprocessRule(1, new NegDeg01Type());
  addPreprocessRule(1, new PosEdgeType());
  addPreprocessRule(1, new NegEdgeType());
  addPreprocessRule(1, new NegCircuitType());
  addPreprocessRule(1, new NegDiamondType());
  addPreprocessRule(1, new PosDeg01Type());
  addPreprocessRule(1, new PosDiamondType());
  addPreprocessRule(2, new NegMirroredHubsType());
}

template<typename GR, typename NWGHT, typename NLBL, typename EWGHT>
inline MwcsPreprocessedGraph<GR, NWGHT, NLBL, EWGHT>::~MwcsPreprocessedGraph()
{
  for (size_t i = 0; i < _rules.size(); ++i)
  {
    for (RuleVectorNonConstIt it = _rules[i].begin(); it != _rules[i].end(); it++)
    {
      delete *it;
    }
  }
  
  delete _pGraph;
  delete _pBackupGraph;
}

template<typename GR, typename NWGHT, typename NLBL, typename EWGHT>
inline void MwcsPreprocessedGraph<GR, NWGHT, NLBL, EWGHT>::preprocess(const NodeSet& rootNodes)
{
  DegreeNodeMap degree(*_pGraph->_pG);
  DegreeNodeSetVector degreeVector;
  NodeSetMap neighbors(*_pGraph->_pG);
  
  constructDegreeMap(degree, degreeVector);
  constructNeighborMap(neighbors);

  // determine max score
  double LB = std::max((*_pGraph->_pScore)[lemon::mapMax(*_pGraph->_pG, *_pGraph->_pScore)], 0.);
  
  // now let's preprocess the graph
  // in phases: first do phase 0 until no more change
  // then move on to phase 1 upon change fallback to phase 0
  //
  int uberTotRemovedNodes;
  do
  {
    uberTotRemovedNodes = 0;
    for (size_t phase = 0; phase < _rules.size(); ++phase)
    {
      int totRemovedNodes;
      do
      {
        totRemovedNodes = 0;
        for (RuleVectorIt ruleIt = _rules[phase].begin(); ruleIt != _rules[phase].end(); ruleIt++)
        {
          int removedNodes = (*ruleIt)->apply(*_pGraph->_pG, rootNodes,
                                              getArcLookUp(), *_pGraph->_pLabel,
                                              *_pGraph->_pScore, *_pGraph->_pComp, *_pGraph->_pMapToPre,
                                              *_pGraph->_pPreOrigNodes, neighbors,
                                              _pGraph->_nNodes, _pGraph->_nArcs,
                                              _pGraph->_nEdges, _pGraph->_nComponents,
                                              degree, degreeVector, LB);
          totRemovedNodes += removedNodes;

          if (g_verbosity >= VERBOSE_DEBUG && removedNodes > 0)
          {
            std::cout << "// Phase " << phase
                      << ": applied rule '" << (*ruleIt)->name()
                      << "' and removed " << removedNodes
                      << " node(s)" << std::endl;
          }
        }
        
        if (totRemovedNodes > 0)
        {
          phase = 0;
          uberTotRemovedNodes += totRemovedNodes;
        }
      } while (totRemovedNodes > 0);
    }
  } while (uberTotRemovedNodes > 0);

  // determine the connected components
  _pGraph->_nComponents = lemon::connectedComponents(*_pGraph->_pG, *_pGraph->_pComp);

  if (g_verbosity >= VERBOSE_ESSENTIAL)
  {
    std::cout << "// Preprocessing successfully applied"
              << ": " << _pGraph->_nNodes << " nodes, "
              << _pGraph->_nEdges << " edges and "
              << _pGraph->_nComponents << " component(s) remaining" << std::endl;
  }
}

template<typename GR, typename NWGHT, typename NLBL, typename EWGHT>
inline bool MwcsPreprocessedGraph<GR, NWGHT, NLBL, EWGHT>::init(ParserType* pParser, bool pval)
{
  if (!Parent::init(pParser, pval))
    return false;

  // start by making a copy of the graph
  clear();

  return true;
}

template<typename GR, typename NWGHT, typename NLBL, typename EWGHT>
inline bool MwcsPreprocessedGraph<GR, NWGHT, NLBL, EWGHT>::init(Graph* pG,
                                                                LabelNodeMap* pLabel,
                                                                WeightNodeMap* pScore,
                                                                WeightNodeMap* pPVal)
{
  if (!Parent::init(pG, pLabel, pScore, pPVal))
    return false;

  // start by making a copy of the graph
  clear();

  return true;
}

template<typename GR, typename NWGHT, typename NLBL, typename EWGHT>
inline void MwcsPreprocessedGraph<GR, NWGHT, NLBL, EWGHT>::constructNeighborMap(NodeSetMap& neighbors) const
{
  const Graph& g = *_pGraph->_pG;
  for (NodeIt n(g); n != lemon::INVALID; ++n)
  {
    NodeSet& neighborSet = neighbors[n];
    neighborSet.clear();
    for (IncEdgeIt e(g, n); e != lemon::INVALID; ++e)
    {
      neighborSet.insert(g.oppositeNode(n, e));
    }
  }
}
  
template<typename GR, typename NWGHT, typename NLBL, typename EWGHT>
inline void MwcsPreprocessedGraph<GR, NWGHT, NLBL, EWGHT>::constructDegreeMap(
    DegreeNodeMap& degree,
    DegreeNodeSetVector& degreeVector) const
{
  for (NodeIt n(*_pGraph->_pG); n != lemon::INVALID; ++n)
  {
    int d = 0;
    for (IncEdgeIt e(*_pGraph->_pG, n); e != lemon::INVALID; ++e, d++) ;

    degree[n] = d;
    if (degreeVector.size() <= static_cast<size_t>(d))
    {
      // add node sets to degreeVector
      int len = d - degreeVector.size() + 1;
      for (int i = 0; i < len; i++)
        degreeVector.push_back(NodeSet());
    }

    degreeVector[d].insert(n);
  }
}

template<typename GR, typename NWGHT, typename NLBL, typename EWGHT>
inline void MwcsPreprocessedGraph<GR, NWGHT, NLBL, EWGHT>::computeScores(double lambda,
                                                                         double a,
                                                                         double FDR)
{
  Parent::computeScores(lambda, a, FDR);
  clear();
}

template<typename GR, typename NWGHT, typename NLBL, typename EWGHT>
inline void MwcsPreprocessedGraph<GR, NWGHT, NLBL, EWGHT>::computeScores(double tau)
{
  Parent::computeScores(tau);
  clear();
}

} // namespace mwcs
} // namespace nina

#endif // MWCSPREPROCESSEDGRAPH_H
