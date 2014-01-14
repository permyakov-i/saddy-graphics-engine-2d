/*! \file gridnodedge.h
	\author HiddenSeeker

	Describes a basic grid node edge
 */
#include <scene.h>
#include <p2d/body.h>
#include <p2d/app/constants.h>
#include <sprite2dadapter.h>
#pragma once

/*! As a default, grid node is a node
 */
class GridNodeEdge: public sad::SceneNode
{
 SAD_OBJECT
 protected:
	 sad::p2d::Body * m_b1;
	 sad::p2d::Body * m_b2;
	 Sprite2DAdapter * m_ad;
 public:
	 GridNodeEdge(sad::p2d::Body * b1, sad::p2d::Body * b2);

	 virtual void render();

	 virtual ~GridNodeEdge();
};
