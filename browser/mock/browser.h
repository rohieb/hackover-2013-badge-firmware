#ifndef INCLUDED_BROWSER_MOCK_BROWSER_H
#define INCLUDED_BROWSER_MOCK_BROWSER_H

enum {
  BROWSER_CONTINUE,
  BROWSER_EXIT
};

void mock_browse_open_file(char const *fname);
int mock_browse_tick(void);

#endif
