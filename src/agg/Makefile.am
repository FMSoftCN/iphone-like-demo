TOP_DIR=..
INCLUDES = 
SUBDIRS=

lib_LTLIBRARIES = libagg.la

SRC_FILES = agg_flip.cpp  \
	    agg_scroll.cpp \
	    agg_coverflow.cpp \
	    interactive_polygon.cpp  \
	    agg_platform_support.cpp  \
	    agg_trans_affine.cpp \
	    agg_image_filters.cpp \
	    agg_vcgen_stroke.cpp \
	    agg_curves.cpp agg_bspline.cpp agg_vcgen_bspline.cpp 
HDR_FILES =  agg_coverflow.h agg_scroll.h agg_flip.h interactive_polygon.h agg_platform_support.h  \
			 agg_trans_affine.h agg_image_filters.h agg_vcgen_stroke.h \
			 agg_array.h agg_image_filters.h agg_span_allocator.h \
			 agg_basics.h agg_math.h agg_span_image_filter.h \
			 agg_bezier_arc.h agg_math_stroke.h agg_span_image_filter_rgba.h \
			 agg_clip_liang_barsky.h agg_path_storage.h agg_span_image_filter_rgb.h \
			 agg_color_rgba.h agg_pixfmt_rgba.h agg_span_interpolator_linear.h \
			 agg_config.h agg_platform_support.h agg_span_interpolator_trans.h \
			 agg_conv_adaptor_vcgen.h agg_rasterizer_cells_aa.h agg_span_subdiv_adaptor.h \
			 agg_conv_stroke.h agg_rasterizer_scanline_aa.h agg_trans_affine.h \
			 agg_conv_transform.h agg_rasterizer_sl_clip.h agg_trans_bilinear.h \
			 agg_ctrl.h agg_renderer_base.h agg_trans_perspective.h \
			 agg_dda_line.h agg_renderer_scanline.h agg_trans_viewport.h \
			 agg_ellipse.h agg_rendering_buffer.h agg_vcgen_stroke.h \
			 agg_flip.h agg_scanline_u.h agg_vertex_sequence.h \
			 agg_gamma_functions.h agg_scanline_p.h agg_conv_bspline.h \
			 agg_conv_segmentator.h agg_trans_single_path.h agg_span_image_filter_gray.h \
			 agg_vcgen_bspline.h agg_bspline.h agg_conv_adaptor_vpgen.h \
			 agg_shorten_path.h interactive_polygon.h agg_vpgen_segmentator.h \
			 agg_color_gray.h pixel_formats.h agg_pixfmt_rgb.h agg_curves.h \
			 agg_image_accessors.h agg_simul_eq.h

libagg_la_SOURCES = $(SRC_FILES) $(HDR_FILES)
