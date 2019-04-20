#include "notifications.h"
#include <deque>

c_notifications g_notify;

void c_notifications::add( bool display, const ImU32 &color, const std::string message, ... ) {
	if( message.empty( ) )
		return;

	va_list va;
	std::string buf;

	va_start( va, message );

	const int str_len = std::vsnprintf( nullptr, 0, message.c_str( ), va );
	if( str_len < 0 ) {
		va_end( va );

		return;
	}

	buf.resize( str_len );

	std::vsnprintf( &buf[ 0 ], str_len + 1, message.c_str( ), va );

	if( m_notifications.size( ) > 15 )
		m_notifications.pop_front( );

	g_csgo.m_convar->print_to_console( Color( 206, 115, 136 ), "[deadcell]" );
	g_csgo.m_convar->print_to_console( Color( 255, 255, 255 ), " %s\n", buf.c_str( ) );

	if( display )
		m_notifications.emplace_back( color, ImColorARGB( 125, 0, 0, 0 ), g_csgo.m_global_vars->m_cur_time, buf );
}

void c_notifications::clear( ) {
	m_notifications.clear( );
}

void c_notifications::draw( ) {
	int adjust_height = 5;

	for( size_t i = 0; i < m_notifications.size( ); i++ ) {
		auto &notification = m_notifications.at( i );

		ImColorARGB color{ notification.color };
		ImColorARGB shadow_color{ notification.background_color };

		const float cur_time = notification.m_time;
		std::string message = notification.m_buf;

		if( g_csgo.m_global_vars->m_cur_time - cur_time > m_text_duration ) {
			int alpha = static_cast< int >( color.Value.w * 255.f - 255 / 1.f * std::max( g_csgo.m_global_vars->m_frametime, 0.01f ) );
			if( alpha > 255 )
				alpha = 255;
			if( alpha < 0 )
				alpha = 0;

			if( !alpha ) {
				m_notifications.erase( m_notifications.begin( ) + i );
				continue;
			}

			color.Value.w = alpha / 255.0f;
			shadow_color.Value.w = alpha / 255.0f;
		}

		const ImVec2 text_size = g_renderer.get_font( FONT_VERDANA_7PX )->CalcTextSizeA( g_renderer.get_font( FONT_VERDANA_7PX )->FontSize, FLT_MAX, 0.f, message.c_str( ) );

		g_renderer.ansi_text( g_renderer.get_font( FONT_VERDANA_7PX ), color, shadow_color, 8, adjust_height, DROPSHADOW, message );

		adjust_height += static_cast< int >( text_size.y + 1.f );
	}
}
