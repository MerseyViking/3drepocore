/**
 *  Copyright (C) 2014 3D Repo Ltd
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "repo_graph_abstract.h"
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/assign.hpp>
#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

//------------------------------------------------------------------------------
repo::core::RepoGraphAbstract::~RepoGraphAbstract() {}

//------------------------------------------------------------------------------
std::set<const repo::core::RepoNodeAbstract*> 
	repo::core::RepoGraphAbstract::getNodesRecursively() const
{
	std::set<const RepoNodeAbstract*> nodes;
	if (NULL != rootNode)
		rootNode->getSubNodes(nodes);
	return nodes;
}

//------------------------------------------------------------------------------
std::set<repo::core::RepoNodeAbstract*>
    repo::core::RepoGraphAbstract::getNodes() const
{
	std::set<RepoNodeAbstract*> values;
    boost::copy(nodesByUniqueID | boost::adaptors::map_values,
                std::inserter(values, values.begin()));
	return values;
}

//------------------------------------------------------------------------------
std::set<boost::uuids::uuid> repo::core::RepoGraphAbstract::getUniqueIDs() const
{
	std::set<boost::uuids::uuid> keys;
    boost::copy(nodesByUniqueID | boost::adaptors::map_keys,
                std::inserter(keys, keys.begin()));
	return keys;
}

//------------------------------------------------------------------------------
repo::core::RepoNodeAbstract*  repo::core::RepoGraphAbstract::getNodeByUniqueID(
	const boost::uuids::uuid& uid) const
{
	RepoNodeAbstract* node = NULL;
	std::map<boost::uuids::uuid, RepoNodeAbstract*>::const_iterator it =
		nodesByUniqueID.find(uid);
	if (nodesByUniqueID.end() != it)		
		node = it->second;
	return node;
}

//------------------------------------------------------------------------------
repo::core::RepoNodeAbstract* repo::core::RepoGraphAbstract::addNodeByUniqueID(
	RepoNodeAbstract* node)
{
	RepoNodeAbstract* oldNode = NULL;
	if (node)
	{
		boost::uuids::uuid uid = node->getUniqueID();
        std::pair<std::map<boost::uuids::uuid,RepoNodeAbstract*>::iterator,bool>
                ret = nodesByUniqueID.insert(std::make_pair(uid, node));

		// If there was a previous entry with the same UID (insertion failed), 
		// report it as oldNode and replace it with the new given node.
		if (!ret.second) {
			oldNode = ret.first->second;
			ret.first->second = node;
		}
	}
	return oldNode;
}

//------------------------------------------------------------------------------
void repo::core::RepoGraphAbstract::buildGraph(
	const std::map<boost::uuids::uuid, RepoNodeAbstract*>& nodesBySharedID) const
{
	std::map<boost::uuids::uuid, RepoNodeAbstract*>::const_iterator it;		
	std::map<boost::uuids::uuid, RepoNodeAbstract*>::const_iterator finder;	

	for (it = nodesBySharedID.begin(); it!= nodesBySharedID.end(); ++it)
	{
		RepoNodeAbstract* node = it->second;		

        std::set<const boost::uuids::uuid> psi = node->getParentSharedIDs();
        for (std::set<const boost::uuids::uuid>::iterator::it = psi.begin();
             it != psi.end(); ++it)
        {
            boost::uuids::uuid = *it;
            finder = nodesBySharedID.find(uuid);
            if (nodesBySharedID.end() != finder)
            {
                RepoNodeAbstract* parent = finder->second;
                parent->addChild(node);
                node->addParent(parent);
            }
        }

//		for each (const boost::uuids::uuid uuid in node->getParentSharedIDs())
//		{
//			finder = nodesBySharedID.find(uuid);
//			if (nodesBySharedID.end() != finder)
//			{
//				RepoNodeAbstract* parent = finder->second;
//				parent->addChild(node);
//				node->addParent(parent);
//			}
//		}
	}
}