/*! \file customobject.h
	

	Describes custom object with custom properties. Could be Sprite2D or Labels, depending on stuff
 */
#pragma once
#include "../../scenenode.h"
#include "../../resource/link.h"
#include "../../resource/tree.h"
#include "customschema.h"


namespace sad
{
class Sprite2D;
class Label;

namespace db
{
	
namespace custom
{

/*! Defines an extensible object as scene node, that could 
	be represented as either Sprite2D, or Label 
 */
class Object: public sad::SceneNode
{
SAD_OBJECT
public:
	/*! Creates new empty stub object
	 */
	Object();
	/*! Deletes own schema and data
	 */
	virtual ~Object();
	/*! Sets scene 
		\param[in] scene a scene, which will render a node
	 */
	virtual void setScene(sad::Scene * scene);
	/*! Whether inner object is one of specified types
		\param[in] type a name of type
		\return whether it is of specified type
	 */
	bool innerTypeIs(const sad::String& type);
	/*! Sets a tree name for object with specified renderer
		\param[in] r renderer, which tree should be fetched from
		\param[in] tree_name a name for an item for object
	 */
	virtual void setTreeName(sad::Renderer* r, const sad::String & tree_name);
	/*! Returns schema for an object
		\return schema
	 */
	virtual sad::db::schema::Schema* schema() const;
	/*! Fills vector of regions with data, that could be used for identifying bounds of item.
		As default, no regions are produced.
		\param[out] r a vector of regions
	 */
	virtual void regions(sad::Vector<sad::Rect2D> & r);
	/*! Returns fully rotated region for object
		\return region
	 */
	virtual sad::Rect2D renderableArea() const;
	/*! Renders a current rendered object
	 */ 
	virtual void render();
	/*! Sets new schema name for an object
		\param[in] s a string name
	 */
	void setSchemaName(const sad::String & s);
	/*! Returns schema path as path to resource 
		\return schema path to resource
	 */
	const sad::String& schemaName() const;
	/*! Returns size of font, used in label
		\return font size
	 */
	unsigned int fontSize() const;
	/*! Sets size of font, used in label
		\param[in] size a size of custom object
	 */
	void setFontSize(unsigned int);
	/*! Sets font name for custom object
		\return font name
	 */
	const sad::String& fontName() const;
	/*! Sets font name for object
		\param[in] name name for font
	 */
	void setFontName(const sad::String& name);
	/*! Returns line spacing for, used in label
		\return line spacing for label
	 */ 
	float lineSpacing() const;
	/*! Sets line spacing, used in label
		\param[in] s a line spacing
	 */
	void setLineSpacing(float s);
    /*! Sets maximal line width, used in label
        \param[in] width a width of object
     */
    void setMaximalLineWidth(unsigned int width);
	/*! Returns angle, used in object
		\return angle in object
	 */
	double angle() const;
	/*! Sets angle
		\param[in] a object rotation angle
	 */
	void setAngle(double a);
	/*! Returns non-rotated object area
		\return non-rotated object area
	 */
	sad::Rect2D area() const;
	/*! Sets non-rotated object area
		\param[in] object area
	 */
	void setArea(const sad::Rect2D & area);
	/*! Returns a string for label
		\return string data
	 */
	const sad::String& string() const;
	/*! Sets a string for a label
		\param[in] s a string data
	 */
	void setString(const sad::String & s);
	/*! Returns color for sprite and label
		\return color for sprite and label
	 */
	const sad::AColor& color() const;
	/*! Sets color for sprite and label
		\return color
	 */
	void setColor(const sad::AColor & c);
	/*! Whether sprite is flipped horizontally
		\return whether sprite is flipped horizontally
	 */
	bool flipX() const;
	/*! Whether sprite is flipped horizontally 
		\param[in] whether sprite is flipped horizontally
	 */
	void setFlipX(bool f);
	/*! Whether sprite is flipped vertically
		\return whether sprite is flipped horizontally
	 */
	bool flipY() const;
	/*! Whether sprite is flipped vertically 
        \param[in] f flag
	 */
	void setFlipY(bool f);
    /*! Returns maximal line width, used in label
        \return maximal line width
     */
    unsigned int maximalLineWidth() const;
	/*! Whether custom object could be rendered
		\return whether object could be rendered
	 */
	bool canBeRendered() const;
	/*! Returns property list from schema
		\return property list from schema
	 */
	const sad::Hash<sad::String, sad::db::Property*>& schemaProperties() const;
	/*! Sets sprite options for custom object
		\param[in] o options
	 */
	void setOptions(const sad::String& o);
	/*! Returns sprite options for custom object
		\return sprite options
	 */
	const sad::String& options() const;
	/*! Loads object from picojson object
		\param[in] v a picojson object
		\return  whether it as successfull
	 */
	virtual bool load(const picojson::value& v);
	/*! Sets a texture coordinates for sprites
		\param[in] texturecoordinates a texture coordinates for a sprite in notation, defined in 
									  constructor
	 */
	void setTextureCoordinates(const sad::Rect2D & texturecoordinates);
	/*! Returns a texture coordinates
		\return a texture coordinates
	 */
	const sad::Rect2D & textureCoordinates() const;
protected:
	/*! Fills custom object with defailt item properties
	 */
	void initDefaultSchema();
	/*! Updates configuration of object, basing on schema
		\param[in] s a new schema 
	 */
	void updateConfiguration(sad::db::custom::Schema * s);
	/*! A current rendered object for custom object
	 */
	sad::SceneNode * m_current_rendered_object;
	/*! A sprite as defined 
	 */
	sad::Sprite2D * m_sprite2d;
	/*! A label as defined
	 */
	sad::Label * m_label;
	/*! A stored properties schema for object
	 */
	sad::db::schema::Schema * m_custom_schema;
	/*! An object's schema
	 */
	sad::db::schema::Schema * m_my_schema;
	/*! A schema for an object
		\param[in] schema for object
	 */
	sad::resource::Link<sad::db::custom::Schema> m_schema;
};


}

}

}
