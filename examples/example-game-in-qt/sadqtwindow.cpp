#include "sadqtwindow.h"
#include "sadqtrenderer.h"
#include "sadqtopenglwidget.h"


// ======= PUBLIC METHODS =======

sad::qt::Window::Window() : m_fullscreen(false)
{
	
}

sad::qt::Window::~Window()
{

}

bool sad::qt::Window::valid() const
{
	return true;
}

bool sad::qt::Window::fixed() const
{
	return true;
}

void sad::qt::Window::makeFixedSize()
{
	
}

void sad::qt::Window::makeResizeable()
{
	
}

bool sad::qt::Window::fullscreen() const
{
	return m_fullscreen;
}

void sad::qt::Window::enterFullscreen()
{
	QWidget* w = this->window();
	if (w)
	{
		w->window()->showFullScreen();
	}
	m_fullscreen = true;
}

void sad::qt::Window::leaveFullscreen()
{
	QWidget* w = this->window();
	if (w)
	{
		w->window()->showNormal();
	}
	m_fullscreen = true;
}


bool sad::qt::Window::hidden() const
{	
	QWidget* w = this->window();
	if (w)
	{
		return w->isHidden();
	}
	return false;
}


void sad::qt::Window::show()
{
	QWidget* w = this->window();
	if (w)
	{
		return w->show();
	}
}

void sad::qt::Window::hide()
{
	QWidget* w = this->window();
	if (w)
	{
		return w->show();
	}
}



void sad::qt::Window::setRect(const sad::Rect2I& rect)
{
	QWidget* w = this->widget();
	if (w)
	{
		QRect r = w->rect();
		w->setGeometry(r.x(), r.y(), rect.width(), rect.height());
	}
}

void sad::qt::Window::pushRect(const sad::Rect2I& rect)
{
	m_sizes.push(rect);
	setRect(rect);
}

void sad::qt::Window::popRect()
{
	if (m_sizes.size())
	{
		setRect(m_sizes.pop());
	}
}



sad::Rect2I sad::qt::Window::rect() const
{
	QWidget* w = this->widget();
	if (w)
	{
		QRect r = w->rect();
		return sad::Rect2I(r.x(), r.y(), r.x() + r.width(), r.y() + r.height());
	}
	return sad::Rect2I();
}


sad::Point2D sad::qt::Window::toClient(const sad::Point2D & p)
{
	// TODO: Fix this if broken
	return p;
}


sad::os::WindowHandles * sad::qt::Window::handles()
{
	return NULL;
}


const sad::String & sad::qt::Window::title() const
{
	QWidget* window = this->window();
	if (window)
	{
		const_cast<sad::qt::Window*>(this)->m_window_title = window->windowTitle().toStdString().c_str();
	}
	return m_window_title;
}

void sad::qt::Window::setTitle(const sad::String & s)
{
	m_window_title = s;
	QWidget* window = this->window();
	if (window)
	{
		window->setWindowTitle(s.c_str());
	}
}


bool sad::qt::Window::isGL3compatible() const
{
	return true;
}

bool sad::qt::Window::create()
{
	return true;
}

void sad::qt::Window::close()
{
	QWidget* w = this->window();
	if (w)
	{
		w->close();
	}
}

void sad::qt::Window::destroy()
{
	
}

// ======= PROTECTED METHODS =======


QWidget* sad::qt::Window::widget() const
{
	if (this->renderer())
	{
		sad::qt::Renderer* r = static_cast<sad::qt::Renderer*>(this->renderer());
		if (r->widget())
		{
			return r->widget();
		}
	}

	return NULL;
}

QWidget* sad::qt::Window::window() const
{
	QWidget* w = this->widget();
	if (w)
	{
		return w->window();
	}
	return NULL;
}