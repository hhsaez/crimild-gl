/**
 * Crimild Engine is an open source scene graph based engine which purpose
 * is to fulfill the high-performance requirements of typical multi-platform
 * two and tridimensional multimedia projects, like games, simulations and
 * virtual reality.
 *
 * Copyright (C) 2006-2013 Hernan Saez - hhsaez@gmail.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef CRIMILD_GLFW_TASKS_WINDOW_
#define CRIMILD_GLFW_TASKS_WINDOW_

#include <Crimild.hpp>

namespace Crimild {

	class WindowTask : public Task {
	public:
		WindowTask( int priority );
		virtual ~WindowTask( void );

		virtual void start( void ) override;
		virtual void update( void ) override;
		virtual void stop( void ) override;
	};

	typedef std::shared_ptr< WindowTask > WindowTaskPtr;

}

#endif

