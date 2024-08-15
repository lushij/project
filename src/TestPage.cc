#include "PageLib.h"
#include "PageLibprocessor.h"
int main()
{
    PageLib _pagelib;
    _pagelib.create_page_offset();
    _pagelib.write_page_offset();
    PageLibprocessor _PageLibPreprocessor;
    _PageLibPreprocessor.buildInvertIndex();
    _PageLibPreprocessor.writeInvertIndex();
    return 0;
}

