#include "label.h"
#include "geometry2d.h"
#include "renderer.h"
#include "sadmutex.h"

#include "db/schema/schema.h"
#include "db/dbproperty.h"
#include "db/save.h"
#include "db/load.h"
#include "db/dbmethodpair.h"

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

DECLARE_SOBJ_INHERITANCE(sad::Label,sad::SceneNode)


#define DEBUG_FORMATTED_RENDERING 1

sad::Label::Label() :
m_string(""),
m_maximal_line_width(0), 
m_point(0, 0), m_angle(0), 
m_size(20), m_linespacing_ratio(1.0),
m_color(0, 0, 0, 0),
m_overflow_strategy(sad::Label::LOS_VISIBLE),
m_break_text(sad::Label::LBT_NORMAL),
m_text_ellipsis_position(sad::Label::LTEP_MIDDLE),
m_maximum_lines(0),
m_overflow_strategy_for_lines(sad::Label::LOS_VISIBLE),
m_text_ellipsis_position_for_lines(sad::Label::LTEP_MIDDLE),
m_formatted(false),
m_computed_rendering_string(false),
m_computed_rendering_point(false)
{
    
}

sad::Label::Label(
    sad::Font *  font,
    const sad::Point2D  & point,
    const sad::String & string
) : 
m_string(string), 
m_maximal_line_width(0), m_point(point),  m_angle(0), 
m_size(20), m_linespacing_ratio(1.0),
m_color(0, 0, 0, 0),
m_overflow_strategy(sad::Label::LOS_VISIBLE),
m_break_text(sad::Label::LBT_NORMAL),
m_text_ellipsis_position(sad::Label::LTEP_MIDDLE),
m_maximum_lines(0),
m_overflow_strategy_for_lines(sad::Label::LOS_VISIBLE),
m_text_ellipsis_position_for_lines(sad::Label::LTEP_MIDDLE),
m_formatted(false),
m_computed_rendering_string(false),
m_computed_rendering_point(false)
{
    m_font.attach(font);
    recomputeRenderedString();
}

sad::Label::Label(
    const sad::String &  font,
    const sad::Point2D  & point,
    const sad::String & string,
    const sad::String & tree
) :
m_string(string), 
m_maximal_line_width(0), m_point(point),  m_angle(0), 
m_size(20), m_linespacing_ratio(1.0),
m_color(0, 0, 0, 0),
m_overflow_strategy(sad::Label::LOS_VISIBLE),
m_break_text(sad::Label::LBT_NORMAL),
m_text_ellipsis_position(sad::Label::LTEP_MIDDLE),
m_maximum_lines(0),
m_overflow_strategy_for_lines(sad::Label::LOS_VISIBLE),
m_text_ellipsis_position_for_lines(sad::Label::LTEP_MIDDLE),
m_formatted(false),
m_computed_rendering_string(false),
m_computed_rendering_point(false)
{
    m_font.setTree(NULL, tree);
    m_font.setPath(font);
    recomputeRenderedString();
}

void sad::Label::setTreeName(sad::Renderer* r, const sad::String& treename)
{
    m_font.setTree(r, treename);
    for (sad::Hash<sad::String, sad::resource::Link<sad::Font>>::iterator it = m_fonts_for_document.begin(); it != m_fonts_for_document.end(); ++it)
    {
        it.value().setTree(r, treename);
    }
}

void sad::Label::regions(sad::Vector<sad::Rect2D> & r)
{
    r << m_cached_region;
}

static sad::db::schema::Schema* LabelBasicSchema = NULL;

static sad::Mutex LabelBasicSchemaInit;

sad::db::schema::Schema* sad::Label::basicSchema()
{
    if (LabelBasicSchema == NULL)
    {
        LabelBasicSchemaInit.lock();
        if (LabelBasicSchema == NULL)
        {
            LabelBasicSchema = new sad::db::schema::Schema();
            LabelBasicSchema->addParent(sad::SceneNode::basicSchema());
            LabelBasicSchema->add(
                "font", 
                new sad::db::MethodPair<sad::Label, sad::String>(
                    &sad::Label::fontName,
                    &sad::Label::setFontName
                )
            );
            LabelBasicSchema->add(
                "fontsize", 
                new sad::db::MethodPair<sad::Label, unsigned int>(
                    &sad::Label::size,
                    &sad::Label::setSize
                )
            );
            LabelBasicSchema->add(
                "linespacing", 
                new sad::db::MethodPair<sad::Label, float>(
                    &sad::Label::lineSpacingRatio,
                    &sad::Label::setLineSpacingRatio
                )
            );
            LabelBasicSchema->add(
                "angle", 
                new sad::db::MethodPair<sad::Label, double>(
                    &sad::Label::angle,
                    &sad::Label::setAngle
                )
            );
            LabelBasicSchema->add(
                "area", 
                new sad::db::MethodPair<sad::Label, sad::Rect2D>(
                    &sad::Label::area,
                    &sad::Label::setArea
                )
            );
            LabelBasicSchema->add(
                "text", 
                new sad::db::MethodPair<sad::Label, sad::String>(
                    &sad::Label::string,
                    &sad::Label::setString
                )
            );
            void (sad::Label::*p)(const sad::AColor&)= &sad::Label::setColor;
            LabelBasicSchema->add(
                "color", 
                new sad::db::MethodPair<sad::Label, sad::AColor>(
                    &sad::Label::color,
                    p
                )
            );

            sad::db::Property* mlw_property = new sad::db::MethodPair<sad::Label, unsigned int>(
                &sad::Label::maximalLineWidth,
                &sad::Label::setMaximalLineWidth
            );
            mlw_property->makeNonRequiredWithDefaultValue(new sad::db::Variant(static_cast<unsigned int>(0)));
            LabelBasicSchema->add("maximallinewidth", mlw_property);

            sad::db::Property* os_property = new sad::db::MethodPair<sad::Label, unsigned int>(
                &sad::Label::overflowStrategyAsIndex,
                &sad::Label::setOverflowStrategyFromIndex
            );
            os_property->makeNonRequiredWithDefaultValue(new sad::db::Variant(static_cast<unsigned int>(0)));
            LabelBasicSchema->add("overflowstrategy", os_property);

             sad::db::Property* bt_property = new sad::db::MethodPair<sad::Label, unsigned int>(
                &sad::Label::breakTextAsIndex,
                &sad::Label::setBreakTextFromIndex
            );
            bt_property->makeNonRequiredWithDefaultValue(new sad::db::Variant(static_cast<unsigned int>(0)));
            LabelBasicSchema->add("breaktext", bt_property);

            sad::db::Property* tep_property = new sad::db::MethodPair<sad::Label, unsigned int>(
                &sad::Label::textEllipsisAsIndex,
                &sad::Label::setTextEllipsisPositionAsIndex
            );
            tep_property->makeNonRequiredWithDefaultValue(new sad::db::Variant(static_cast<unsigned int>(0)));
            LabelBasicSchema->add("textellipsisposition", tep_property);


            sad::db::Property* mlines_property = new sad::db::MethodPair<sad::Label, unsigned int>(
                &sad::Label::maximalLinesCount,
                &sad::Label::setMaximalLinesCount
            );
            mlines_property->makeNonRequiredWithDefaultValue(new sad::db::Variant(static_cast<unsigned int>(0)));
            LabelBasicSchema->add("maximallinescount", mlines_property);

            sad::db::Property* oslines_property = new sad::db::MethodPair<sad::Label, unsigned int>(
                &sad::Label::overflowStrategyForLinesAsIndex,
                &sad::Label::setOverflowStrategyForLinesFromIndex
            );
            oslines_property->makeNonRequiredWithDefaultValue(new sad::db::Variant(static_cast<unsigned int>(0)));
            LabelBasicSchema->add("overflowstrategyforlines", oslines_property);

            sad::db::Property* teplines_property = new sad::db::MethodPair<sad::Label, unsigned int>(
                &sad::Label::textEllipsisForLinesAsIndex,
                &sad::Label::setTextEllipsisPositionForLinesAsIndex
            );
            teplines_property->makeNonRequiredWithDefaultValue(new sad::db::Variant(static_cast<unsigned int>(0)));
            LabelBasicSchema->add("textellipsispositionforlines", teplines_property);

            sad::db::Property* hasformatting_property = new sad::db::MethodPair<sad::Label, bool>(
                &sad::Label::hasFormatting,
                &sad::Label::setHasFormatting
                );
            teplines_property->makeNonRequiredWithDefaultValue(new sad::db::Variant(false));
            LabelBasicSchema->add("hasformatting", hasformatting_property);


            sad::ClassMetaDataContainer::ref()->pushGlobalSchema(LabelBasicSchema);
        }
        LabelBasicSchemaInit.unlock();
    }
    return LabelBasicSchema;
}

sad::db::schema::Schema* sad::Label::schema() const
{
    return sad::Label::basicSchema();
}

void sad::Label::render()
{
    sad::Font * font = m_font.get();
    if (!font)
        return;

    font->setSize(m_size);
    font->setColor(m_color);
    font->setLineSpacingRatio(m_linespacing_ratio);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(static_cast<GLfloat>(m_center.x()), static_cast<GLfloat>(m_center.y()), 0.0f);
    glRotatef(static_cast<GLfloat>(m_angle / M_PI * 180.0f), 0.0f, 0.0f, 1.0f);

    if (m_size > 0)
    {
        if (font)
        {
            if (m_formatted)
            {
                renderWithFormatting(font);
            }
            else
            {
                renderWithoutFormatting(font);
            }
        }
    }
    glPopMatrix();
}


void sad::Label::rendererChanged()
{
    if (m_font.dependsOnRenderer())
    {
        m_font.setRenderer(this->renderer());
        sad::Hash<sad::String, sad::resource::Link<sad::Font> >::iterator it = m_fonts_for_document.begin();
        for (; it != m_fonts_for_document.end(); ++it)
        {
            it.value().setRenderer(this->renderer());
        }
    }
}

void sad::Label::setArea(const sad::Rect2D & r)
{
    m_point = r[0];
    recomputeRenderingPoint();
}

sad::Rect2D sad::Label::area() const
{
    // Preserve linkage to a renderer
    if (m_computed_rendering_point == false)
    {
        const_cast<sad::Label *>(this)->recomputeRenderingPoint();
    }
    sad::Size2D  size(m_halfpadding.x() * (-2), m_halfpadding.y() * 2);
    if (m_computed_rendering_point == false)
    {
        size.Width = 0;
        size.Height = 0;
    }

    sad::Rect2D  result(m_point.x(), 
                        m_point.y(), 
                        m_point.x() + size.Width,
                        m_point.y() - size.Height);
    return result;
}

sad::Rect2D sad::Label::region() const
{
    sad::Rect2D result = this->area();
    sad::rotate(result, static_cast<float>(m_angle));
    return result;
}



sad::Label::~Label()
{
    clearFontsCache();
}

void sad::Label::setScene(sad::Scene * scene)
{
    this->sad::SceneNode::setScene(scene);
    if (m_font.dependsOnRenderer() && scene)
    {
        m_font.setRenderer(scene->renderer());
        reloadFont();
    }
}

void sad::Label::setPoint(const sad::Point2D & point)
{
    m_point = point;
    recomputeRenderingPoint();
}

void sad::Label::setFontName(const sad::String & name)
{
    m_font.setPath(name);
    reloadFont();
}

void sad::Label::setFont(sad::Font * font)
{
    m_font.attach(font);
    recomputeRenderingPoint();
}

void sad::Label::setFont(const sad::String & name, sad::Renderer * r, const sad::String & tree)
{
    if (!r)
        r = sad::Renderer::ref();
    m_font.setPath(name);
    m_font.setTree(r, tree);
}

void sad::Label::setString(const sad::String & string)
{
    m_string = string;
    recomputeRenderedString();
}

void sad::Label::setSize(unsigned int size)
{
    m_size = size;
    recomputeRenderingPoint();
}

float sad::Label::builtinLineSpacing() const
{
    // 0.1f is placed to avoid division by zero
    sad::Font * font = m_font.get();
    if (!font)
        return 0.1f;
    font->setSize(m_size);
    return font->builtinLineSpacing();
}

void sad::Label::setLineSpacing(float spacing)
{
    setLineSpacingRatio(spacing / this->builtinLineSpacing());
}

void sad::Label::setLineSpacingRatio(float ratio)
{
    m_linespacing_ratio = ratio;
    recomputeRenderingPoint();
}

void sad::Label::setTreeName(const sad::String & treename)
{
    m_font.setTree(m_font.renderer(), treename);
    sad::Hash<sad::String, sad::resource::Link<sad::Font> >::iterator it = m_fonts_for_document.begin();
    for (; it != m_fonts_for_document.end(); ++it)
    {
        it.value().setTree(m_font.renderer(), treename);
    }
    recomputeRenderingPoint();
}

void sad::Label::setMaximalLineWidth(unsigned int width)
{
    m_maximal_line_width = width;
    recomputeRenderedString();
}

unsigned int sad::Label::maximalLineWidth() const
{
    return m_maximal_line_width;
}

void sad::Label::setOverflowStrategy(sad::Label::OverflowStrategy s)
{
    m_overflow_strategy = s;
    recomputeRenderedString();
}

void sad::Label::setOverflowStrategyFromIndex(unsigned int s)
{
    if (s > sad::Label::LOS_ELLIPSIS)
    {
        s = static_cast<unsigned int>(sad::Label::LOS_ELLIPSIS);
    }
    setOverflowStrategy(static_cast<sad::Label::OverflowStrategy>(s));
}

sad::Label::OverflowStrategy sad::Label::overflowStrategy() const
{
    return m_overflow_strategy;
}

unsigned int sad::Label::overflowStrategyAsIndex() const
{
    return static_cast<unsigned int>(overflowStrategy());
}

void sad::Label::setBreakText(sad::Label::BreakText value)
{
    m_break_text = value;
    recomputeRenderedString();
}


void sad::Label::setBreakTextFromIndex(unsigned int value)
{
    if (value > sad::Label::LBT_BREAK_WORD)
    {
        value = static_cast<unsigned int>(sad::Label::LBT_BREAK_WORD);
    }
    setBreakText(static_cast<sad::Label::BreakText>(value));
}

sad::Label::BreakText sad::Label::breakText() const
{
    return m_break_text;
}

unsigned int sad::Label::breakTextAsIndex() const
{
    return static_cast<unsigned int>(breakText());
}

void sad::Label::setTextEllipsisPosition(sad::Label::TextEllipsisPosition value)
{
    m_text_ellipsis_position = value;
    recomputeRenderedString();
}

void sad::Label::setTextEllipsisPositionAsIndex(unsigned int value)
{
    if (value > sad::Label::LTEP_END)
    {
        value = static_cast<unsigned int>(sad::Label::LTEP_END);
    }
    setTextEllipsisPosition(static_cast<sad::Label::TextEllipsisPosition>(value));
}

sad::Label::TextEllipsisPosition sad::Label::textEllipsis() const
{
    return m_text_ellipsis_position;
}

unsigned int sad::Label::textEllipsisAsIndex() const
{
    return static_cast<unsigned int>(textEllipsis());
}

void sad::Label::setMaximalLinesCount(unsigned int line_count)
{
    m_maximum_lines = line_count;
    recomputeRenderedString();
}

unsigned int sad::Label::maximalLinesCount() const
{
    return m_maximum_lines;
}

void sad::Label::setOverflowStrategyForLines(sad::Label::OverflowStrategy s)
{
    m_overflow_strategy_for_lines = s;
    recomputeRenderedString();
}

void sad::Label::setOverflowStrategyForLinesFromIndex(unsigned int s)
{
    if (s > sad::Label::LOS_ELLIPSIS)
    {
        s = static_cast<unsigned int>(sad::Label::LOS_ELLIPSIS);
    }
    setOverflowStrategyForLines(static_cast<sad::Label::OverflowStrategy>(s));    
}

sad::Label::OverflowStrategy sad::Label::overflowStrategyForLines() const
{
    return m_overflow_strategy_for_lines;
}

unsigned int sad::Label::overflowStrategyForLinesAsIndex() const
{
    return static_cast<unsigned int>(overflowStrategyForLines());
}

void sad::Label::setTextEllipsisPositionForLines(sad::Label::TextEllipsisPosition value)
{
    m_text_ellipsis_position_for_lines = value;
    recomputeRenderedString();
}

void sad::Label::setTextEllipsisPositionForLinesAsIndex(unsigned int value)
{
    if (value > sad::Label::LTEP_END)
    {
        value = static_cast<unsigned int>(sad::Label::LTEP_END);
    }
    setTextEllipsisPositionForLines(static_cast<sad::Label::TextEllipsisPosition>(value));
}

sad::Label::TextEllipsisPosition sad::Label::textEllipsisForLines() const
{
    return m_text_ellipsis_position_for_lines;
}

unsigned int sad::Label::textEllipsisForLinesAsIndex() const
{
    return static_cast<unsigned int>(textEllipsisForLines());
}

bool sad::Label::hasFormatting() const
{
    return m_formatted;
}

void sad::Label::setHasFormatting(bool value)
{
    m_formatted = value;
    recomputeRenderedString();
}

void sad::Label::makeFormatted()
{
    setHasFormatting(true);
}

void sad::Label::disableFormatting()
{
    setHasFormatting(false);
}

void sad::Label::makeSpanBetweenPoints(
    const sad::Point2D & p1, 
    const sad::Point2D & p2
)
{
    sad::Font * font = m_font.get();
    if (font) {
        sad::Size2D  size = font->size(m_rendered_string);
        double dy2 = p2.y() - p1.y();
        double dx2 = p2.x() - p1.x();
        double angle2 = 0;
        if (sad::is_fuzzy_zero(dx2) == false || sad::is_fuzzy_zero(dy2) == false)
        {
            angle2 = atan2(dy2, dx2);
            if (angle2 < 0)
            {
                angle2 += 2 * M_PI;
            }
        }
        setPoint((p1.x() + p2.x() - size.Width) / 2, (p1.y() + p2.y() + size.Height) / 2);
        setAngle(angle2);
    }
}

sad::String sad::Label::makeRenderingString(
    const sad::String& string,   
    unsigned int maximal_line_width,
    sad::Label::OverflowStrategy s,
    sad::Label::BreakText bt,
    sad::Label::TextEllipsisPosition tep,
    unsigned int maximum_lines,
    sad::Label::OverflowStrategy overflow_for_lines,
    sad::Label::TextEllipsisPosition text_ellipsis_for_lines
)
{
    sad::String result;
    if (maximal_line_width == 0 && maximum_lines == 0)
    {
        result = string;
    } 
    else
    {
        bool has_n =  string.getLastOccurence("\n") != -1;
        bool has_r =  string.getLastOccurence("\r") != -1;
        sad::String tmp = string;
        if (has_n && has_r)
        {
            // Windows string preprocessing
            tmp.replaceAllOccurences("\r", "");
        } 
        else
        {
            // MacOSX 
            if (!has_n && has_r)
            {
                tmp.replaceAllOccurences("\r", "\n");
            }
        }
        bool last_line_changed = false;
        sad::StringList lines = string.split("\n", sad::String::KEEP_EMPTY_PARTS);
        sad::StringList new_lines;
        if (maximal_line_width == 0)
        {
            new_lines = lines;
            last_line_changed = true;
        }
        for(size_t i = 0; i < lines.size() && (maximal_line_width != 0); i++)
        {
            if (bt == sad::Label::LBT_BREAK_WORD)
            {
                sad::StringList words = lines[i].split(' ');
                new_lines << "";
                sad::String* current_word = &(new_lines[new_lines.size() - 1]);			
                for(size_t j = 0; j <  words.size(); j++)
                {
                    // In case that word  is larger than line width - we can do nothing to prevent it from 
                    // becoming larger
                    sad::String candidate_word = words[j];
                    candidate_word.replaceAllOccurences("\n", "");
                    if (candidate_word.length() > maximal_line_width && current_word->length() == 0)
                    {
                        *current_word = sad::Label::formatTextLine(
                            candidate_word,
                            maximal_line_width,
                            s,
                            tep
                        );
                        new_lines << "";
                        current_word = &(new_lines[new_lines.size() - 1]);
                        last_line_changed = false;
                    }
                    else
                    {
                        int offset = (current_word->length() == 0) ? 0 : 1; 
                        if (current_word->length() + offset + candidate_word.length() > maximal_line_width)
                        {
                            new_lines << "";
                            current_word = &(new_lines[new_lines.size() - 1]);								
                            --j;
                            last_line_changed = false;
                        }
                        else
                        {
                            if (current_word->length())
                            {
                                *current_word += " ";
                            }
                            *current_word += candidate_word;
                            last_line_changed = true;
                        }
                    }
                }
            }
            if (bt == sad::Label::LBT_NORMAL)
            {
                sad::String line = lines[i];
                line.replaceAllOccurences("\n", "");
                new_lines << sad::Label::formatTextLine(
                    line,
                    maximal_line_width,
                    s,
                    tep
                );                
            }
        }
        if (bt == sad::Label::LBT_BREAK_WORD && new_lines.size() != 0 && !last_line_changed)
        {
            new_lines.removeAt(new_lines.size() - 1);
        }
        if (maximum_lines != 0 
            && new_lines.size() > maximum_lines 
            && overflow_for_lines != sad::Label::LOS_VISIBLE)
        {
            if (overflow_for_lines == sad::Label::LOS_HIDDEN)
            {
                new_lines.removeRange(maximum_lines, new_lines.size() - 1);
            }
            if (overflow_for_lines == sad::Label::LOS_ELLIPSIS)
            {
                if (text_ellipsis_for_lines == sad::Label::LTEP_BEGIN)
                {
                    new_lines.removeRange(0, new_lines.size() - maximum_lines - 1);                    
                    new_lines[0] = "...";
                }
                if (text_ellipsis_for_lines == sad::Label::LTEP_END)
                {
                    new_lines.removeRange(maximum_lines, new_lines.size() - 1);                    
                    new_lines[new_lines.size() - 1] = "...";
                }
                if (text_ellipsis_for_lines == sad::Label::LTEP_MIDDLE)
                {
                    long left_length = (maximum_lines - 1) / 2 + (maximum_lines - 1) % 2;
                    long right_length = (maximum_lines - 1) / 2;
                    if (new_lines.size() == 1)
                    {
                        new_lines[0] = "...";
                    }
                    else
                    {
                        if (new_lines.size() == 2)
                        {
                            new_lines[1] = "...";
                        }
                        else
                        {
                            new_lines.removeRange(left_length + 1, new_lines.size() - right_length - 1);
                            new_lines[left_length] = "...";
                        }
                    }
                }
            }
        }
        result = sad::join(new_lines, "\n");
    }
    return result;
}

sad::String sad::Label::formatTextLine(
    const sad::String& string,
    unsigned int maximal_line_width,
    sad::Label::OverflowStrategy s,
    sad::Label::TextEllipsisPosition tep
)
{
    sad::String result = string;
    if (string.length() > maximal_line_width && s != sad::Label::LOS_VISIBLE && maximal_line_width != 0)
    {
        if (s == sad::Label::LOS_HIDDEN)
        {
            result = string.subString(0, maximal_line_width);
        }

        if (s == sad::Label::LOS_ELLIPSIS)
        {
            if (maximal_line_width <= 3)
            {
                switch(maximal_line_width)
                {
                 case 1: { result = "."; break;}
                 case 2: { result = ".."; break;}
                 case 3: { result = "..."; break;}
                }
            }
            else
            {
                if (string.consistsOfWhitespaceCharacters())
                {
                    result = "";
                }
                else
                {
                    if (tep == sad::Label::LTEP_BEGIN)
                    {
                        result = "...";                        
                        sad::String part = string.substr(string.length() - maximal_line_width + 3, maximal_line_width - 3);
                        part.trim();
                        // Clear string, that consists from spaces
                        if (part.length())
                        {
                            result += part;
                        }
                    }
                    // In case of string with length 4 we should use this strategy too to simplify computation
                    if (tep == sad::Label::LTEP_END || (maximal_line_width == 4 && tep == sad::Label::LTEP_MIDDLE))
                    {
                        result = string.substr(0, maximal_line_width - 3);
                        result.trim();
                        result += "...";  
                    }

                    if (tep == sad::Label::LTEP_MIDDLE && maximal_line_width > 4)
                    {
                        int halfwidth = (maximal_line_width - 3) / 2;
                        int halfmod = (maximal_line_width - 3) % 2;
                        sad::String left = string.substr(0, halfwidth);
                        sad::String right = string.getRightPart(halfwidth + halfmod);
                        left.trimRight();
                        right.trimLeft();
                        if (left.length() == 0 && right.length() == 0)
                        {
                            result = "...";
                        }
                        else
                        {
                            result = left;
                            result += "...";
                            result += right;
                        }
                    }
                }
            }
        }
    }
    return result;
}

void sad::Label::moveBy(const sad::Point2D& p)
{
    setPoint(point() + p);
}

void sad::Label::reloadFont()
{
    sad::Font * font = m_font.get();
    if (font)
    {
        recomputeRenderedString();
    }
}


void sad::Label::recomputeRenderingPoint()
{
    m_computed_rendering_point = false;

    sad::Font * font = m_font.get();
    if (!font)
        return;

    sad::Size2D size;
    if (m_formatted)
    {
        size = this->getSizeWithFormatting(font);
    }
    else
    {
        size = this->getSizeWithoutFormatting(font);
    }

    m_center.setX(m_point.x() + size.Width / 2);
    m_center.setY(m_point.y() - size.Height / 2);
    m_halfpadding.setX(size.Width / -2.0);
    m_halfpadding.setY(size.Height / 2.0);

    m_computed_rendering_point = true;

    m_cached_region = this->region();
}

sad::Size2D sad::Label::getSizeWithoutFormatting(sad::Font* font)
{
    return font->size(m_rendered_string);	
}

sad::Size2D sad::Label::getSizeWithFormatting(sad::Font* font)
{
    sad::Size2D result(0, 0);
    m_document_metrics.clear();
    sad::Vector<float> rowmaxs;
    for (size_t row = 0; row < m_document.size(); row++)
    {
        float maxascender = 0;
        float maxlinespacingvalue = 0;
        float width = 0;
        float ymaxx = 0;
        bool last_line = (row == m_document.size() - 1);
        for (size_t i = 0; i < m_document[row].size(); i++)
        {
            sad::util::Markup::Command& c = m_document[row][i];
            sad::Pair<sad::Font*, sad::Font::RenderFlags> pair = this->applyFontCommand(font, c);
            sad::Font* fnt = pair.p1();
            sad::Font::RenderFlags flags = pair.p2();
            // Last line
            if (last_line)
            {
                fnt->setLineSpacingRatio(1);
            }
            sad::Size2D sz = fnt->size(c.Content, flags);
            c.Ascender = fnt->ascent();
            c.LineSpacingValue = fnt->lineSpacing();
            c.Width = sz.Width;
            float localymax = result.Height + fnt->builtinLineSpacing();
            float ascentmax = result.Height + fnt->ascent();
            if (c.Underlined)
            {
                ascentmax += 2;
            }
            ymaxx = (ymaxx > localymax) ? ymaxx : localymax;
            ymaxx = (ymaxx > ascentmax) ? ymaxx : ascentmax;
            maxascender = (maxascender > c.Ascender) ? maxascender : c.Ascender; 
            maxlinespacingvalue = (maxlinespacingvalue > c.LineSpacingValue) ? maxlinespacingvalue : c.LineSpacingValue;
            width += c.Width;
        }
        rowmaxs << ymaxx;
        sad::Label::FormattedRowMetrics metrics;
        metrics.Ascender = maxascender;
        metrics.LineSpacingValue = maxlinespacingvalue;
        metrics.Width = width;
        m_document_metrics << metrics;

        result.Height += metrics.LineSpacingValue;
        result.Width = (result.Width > metrics.Width) ? result.Width : metrics.Width;
    }

    rowmaxs << result.Height;
    result.Height = *(std::max_element(rowmaxs.begin(), rowmaxs.end()));
    
    return result;
}


void sad::Label::recomputeRenderedString()
{
    m_computed_rendering_string = false;

    if (m_formatted)
    {
        this->recomputeRenderingStringWithFormatting();
    }
    else
    {
        this->recomputeRenderingStringWithoutFormatting();
    }

    if (m_computed_rendering_point)
        m_computed_rendering_string = true;
}

void sad::Label::recomputeRenderingStringWithoutFormatting()
{
    m_rendered_string = sad::Label::makeRenderingString(
        m_string,
        m_maximal_line_width,
        m_overflow_strategy,
        m_break_text,
        m_text_ellipsis_position,
        m_maximum_lines,
        m_overflow_strategy_for_lines,
        m_text_ellipsis_position_for_lines
    );
    recomputeRenderingPoint();
}

void sad::Label::recomputeRenderingStringWithFormatting()
{
    m_document = sad::util::Markup::parseDocument(m_string, sad::util::Markup::Command());
    recomputeRenderingPoint();
}

void sad::Label::clearFontsCache()
{
    m_fonts_for_document.clear();
}

sad::Pair<sad::Font*, sad::Font::RenderFlags> sad::Label::applyFontCommand(sad::Font* font, const sad::util::Markup::Command& c)
{
    sad::Font* fnt = font;
    sad::Font::RenderFlags flags = sad::Font::FRF_None;
    // Apply font
    if (c.Font.exists())
    {
        fnt = this->getFontForDocument(c.Font.value());
    }
    // Apply color
    if (c.Color.exists())
    {
        fnt->setColor(c.Color.value());
    }
    else
    {
        fnt->setColor(m_color);
    }
    // Apply line spacing
    if (c.Linespacing.exists())
    {
        switch (c.Linespacing.value().Type)
        {
        case sad::util::Markup::MLST_PERCENTS: fnt->setLineSpacingRatio(c.Linespacing.value().Size / 100.0); break;
        case sad::util::Markup::MLST_PIXELS: fnt->setLineSpacing(c.Linespacing.value().Size); break;
        };
    }
    else
    {
        fnt->setLineSpacingRatio(m_linespacing_ratio);
    }
    // Apply size
    if (c.Size.exists())
    {
        switch (c.Size.value().Type)
        {
        case sad::util::Markup::MFZST_PIXELS: fnt->setSize(c.Size.value().Size); break;
        case sad::util::Markup::MFZST_POINTS: fnt->setSizeInPoints(c.Size.value().Size); break;
        };
    }
    else
    {
        fnt->setSize(m_size);
    }
    if (c.Bold)
    {
        flags = static_cast<sad::Font::RenderFlags>(flags | sad::Font::FRF_Bold);
    }
    if (c.Italic)
    {
        flags = static_cast<sad::Font::RenderFlags>(flags | sad::Font::FRF_Italic);
    }
    return sad::Pair<sad::Font*, sad::Font::RenderFlags>(fnt,flags);
}

sad::Font* sad::Label::getFontForDocument(const sad::String& s)
{
    if (m_fonts_for_document.contains(s))
    {
        sad::Font* fnt = m_fonts_for_document[s].get();
        if (fnt == NULL)
        {
            return m_font.get();
        }
        else
        {
            return fnt;
        }
    }
    m_fonts_for_document.insert(s, m_font);
    m_fonts_for_document[s].setPath(s);
    sad::Font* fnt = m_fonts_for_document[s].get();
    if (fnt == NULL)
    {
        return m_font.get();
    }
    else
    {
        return fnt;
    }
}

void sad::Label::renderWithoutFormatting(sad::Font* font)
{
    font->render(m_rendered_string, m_halfpadding);
}

void sad::Label::renderWithFormatting(sad::Font* font)
{
    sad::Point2D point = m_halfpadding;
    sad::Renderer* renderer = this->renderer();
    for (size_t row = 0; row < m_document.size(); row++)
    {
        double x = point.x();
        bool last_line = (row == m_document.size() - 1);
        for (size_t i = 0; i < m_document[row].size(); i++)
        {
            sad::util::Markup::Command& c = m_document[row][i];
            sad::Pair<sad::Font*, sad::Font::RenderFlags> pair = this->applyFontCommand(font, c);
            sad::Font* fnt = pair.p1();
            sad::Font::RenderFlags flags = pair.p2();
            // Last line
            if (last_line)
            {
                fnt->setLineSpacingRatio(1);
            }
            double y = point.y() - m_document_metrics[row].Ascender + c.Ascender;
            fnt->render(c.Content, sad::Point2D(x, y), flags);			
            if (renderer)
            {
                if (c.Strikethrough)
                {
                    // We assume that capital letters are bigger than 
                    // lowercase in no more than two times, so dividing ascender by 4 would give us
                    // a position in middle of lower case
                    double ky = point.y() - m_document_metrics[row].Ascender + c.Ascender/ 4;
                    sad::Point2D p1(x, ky), p2(x + c.Width, ky);
                    sad::AColor clr = fnt->color();
                    clr.setA(255 - clr.a());
                    renderer->render()->line(p1, p2, clr);
                }
                if (c.Underlined)
                {
                    double ky = point.y() - m_document_metrics[row].Ascender - 2;
                    sad::Point2D p1(x, ky), p2(x + c.Width, ky);
                    sad::AColor clr = fnt->color();
                    clr.setA(255 - clr.a());
                    renderer->render()->line(p1, p2, clr);
                }
            }
            x += c.Width;
        }

        point.setY(point.y() -  m_document_metrics[row].LineSpacingValue);
    }

#ifdef DEBUG_FORMATTED_RENDERING
    if (renderer)
    {
        renderer->render()->rectangle(sad::Rect2D(m_halfpadding, m_halfpadding * (-1)), sad::AColor(255, 0, 0, 255));
    }
#endif
}