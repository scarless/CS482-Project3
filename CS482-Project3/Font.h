#ifndef FONT_H

#define FONT_H

#include <windows.h>
#include <GL/gl.h>

/* Make this header file work with C and C++ source code. */
#ifdef __cplusplus
extern "C"
{
#endif

	/* Font data structure. */
	typedef struct
	{
		GLuint base;        /* Display list number of first character */
		int    widths[256]; /* Width of each character in pixels */
		int    height;      /* Height of characters */
	} GLFONT;

	/* Prototypes. */
	extern GLFONT	*FontCreate(HDC hdc, const char *typeface,
		int height, int weight, DWORD italic);
	extern void	FontDestroy(GLFONT *font);
	extern void	FontPrintf(GLFONT *font, int align, const char *format, ...);
	extern void	FontPuts(GLFONT *font, const char *s);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !FONT_H */
