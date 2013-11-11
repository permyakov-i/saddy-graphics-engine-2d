#include "window.h"
#include "os/windowimpl.h"

sad::Window::Window() : m_dptr(new sad::os::WindowImpl())
{

}

sad::Window::~Window()
{
	delete m_dptr;
}


void sad::Window::setRenderer(sad::Renderer * renderer)
{
	m_dptr->setRenderer(renderer);
}

sad::Renderer * sad::Window::renderer() const
{
	return m_dptr->renderer();
}

void sad::Window::setCreationSize(const sad::Size2I& size)
{
	m_dptr->setCreationSize(size);
}


bool sad::Window::create()
{
	return m_dptr->create();
}

void sad::Window::destroy()
{
	m_dptr->destroy();
}

bool sad::Window::valid() const
{
	return m_dptr->valid();
}

bool sad::Window::fixed() const
{
	return m_dptr->fixed();
}

void sad::Window::makeFixedSize()
{
	m_dptr->makeFixedSize();
}

void sad::Window::makeResizeable()
{
	m_dptr->makeResizeable();
}

bool sad::Window::fullscreen() const
{
	return m_dptr->fullscreen();
}


void sad::Window::enterFullscreen()
{
	m_dptr->enterFullscreen();
}

void  sad::Window::leaveFullscreen()
{
	m_dptr->leaveFullscreen();
}

bool sad::Window::hidden() const
{
	return m_dptr->hidden();
}

void sad::Window::show()
{
	m_dptr->show();
}

void sad::Window::hide()
{
	m_dptr->hide();
}

void sad::Window::setRect(const sad::Rect2I& rect, bool notify)
{
	m_dptr->setRect(rect, notify);
}

void sad::Window::pushRect(const sad::Rect2I& rect)
{
	m_dptr->pushRect(rect);
}

void sad::Window::popRect()
{
	m_dptr->popRect();
}


sad::Rect2I sad::Window::rect() const
{
	return m_dptr->rect();
}

sad::Point2D sad::Window::toClient(const sad::Point2D & p)
{
	return m_dptr->toClient(p);
}

sad::os::WindowHandles * sad::Window::handles()
{
	return m_dptr->handles();
}

