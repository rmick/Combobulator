#include "StyleSheet.h"
#include <QDebug>

StyleSheet myStyleSheet;


QString StyleSheet::getButtonSelectedCss() const
{
    return ButtonSelectedCss;
}

QString StyleSheet::getButtonUnSelectedCss() const
{
    return ButtonUnSelectedCss;
}

QString StyleSheet::getButtonCheckedCss() const
{
    return ButtonCheckedCss;
}

QString StyleSheet::getButtonUnCheckedCss() const
{
    return ButtonUnCheckedCss;
}

QString StyleSheet::getButtonPressedLastCss() const
{
    return ButtonPressedLastCss;
}

QString StyleSheet::getCurrentCSSstring() const
{
    QString StyleText = CssInside;
    if (currentCSS == CssLight) StyleText = CssOutside;
    if (currentCSS == CssDark)  StyleText = CssInside;
    return StyleText;
}

int StyleSheet::getCurrentCSS() const
{
    return currentCSS;
}

void StyleSheet::setCurrentCSS(int value)
{
    currentCSS = value;
    setSpecials(currentCSS);
}

StyleSheet::StyleSheet()
{	
	setFontSizes();

	//Combobulator Status Indicator LED (QPushbutton) in HostGame window.
    YellowButtonCss =  "background-color: yellow; border-style: outset; border-width: 2px; border-radius: 10px; border-color: grey; min-width: 0; min-height: 0";
    GreenButtonCss  =  "background-color: rgb(0,255,50); border-style: outset; border-width: 2px; border-radius: 10px; border-color: grey; min-width: 0; min-height: 0";

    //CSS stylesheet for DARK interface
    CssInside  =    "QWidget                {background-color: black; color: white; font: normal;}";
    CssInside +=    "QSpinBox               {color: white;} ";
    CssInside +=    "QMessageBox            {font: white;}";   // border: solid white; border-width: 2px; border-radius: 2px;} ";
    CssInside +=    "QMenu                  {font:white; color:white;}";
	CssInside +=							qLabelFont;
	CssInside +=    "QLabel                 {color: white;}";

    CssInside +=    "QListWidget            {color: white; border: solid grey; border-width: 2px; border-radius: 10px;}";
    CssInside +=    "QListWidget::item	    {border-top: 2px solid lightgrey;}";
    CssInside +=    "QListWidget::item	    {border-left: 2px solid lightgrey;}";
    CssInside +=    "QListWidget::item	    {border-radius: 10px}";
    CssInside +=    "QListWidget::item	    {background-color: grey;}";
    CssInside +=    "QListWidget::item	    {color: white}";

    CssInside +=    "QToolBar               {border: grey; border-width: 2px; background-color: rgb(50,50,50); spacing: 20px; padding: 10px;}";
	CssInside +=							qToolButtonFont;
	CssInside +=    "QToolButton            {border-width: 2px; border-radius: 10px; border-color: cyan; background-color: grey; padding: 5px; spacing: 20px;}";
    CssInside +=    "QToolButton:checked    {color: cyan; border-color: cyan;}";
	CssInside +=							qPushButtonFont;
	CssInside +=    "QPushButton            {background-color: grey; border-style: outset; border-width: 2px; border-radius: 10px; border-color: grey; min-width: 50; min-height: 30;}";
    CssInside +=    "QPushButton:disabled   {background-color: rgb(50, 50, 50); color: black;}";
    CssInside +=    "QPushButton:checked    {border-color: cyan; color: cyan;}";
    CssInside +=    "QSlider                {min-height: 35px; background: grey; border-style: outset; border-width: 1px; border-radius: 10px; border-color: black;}";
    CssInside +=    "QSlider::groove:horizontal {border-style: outset; border-width: 2px; border-radius: 10px; border-color: black;}";
    CssInside +=    "QSlider::handle:horizontal {background: grey; border: 1px solid #5c5c5c; width: 18px; margin: -2px 0;";
    CssInside +=                                "border-style: outset; border-width: 2px; border-radius: 5px; border-color: cyan;}";
    CssInside +=    "QSlider:disabled           {background-color: rgb(50, 50, 50);}";
    CssInside +=    "QSlider::handle:disabled   {border-color: grey;}";

		     CssInside +=    "QInputDialog		{border-color: white; border-width: 5px; border-radius: 5px; }";
		     CssInside +=    "QInputDialog		{font-size: 40px; 	    }";

    CssInside +=    "QTableWidget		{color: cyan; alternate-background-color: grey; background-color: black;}";
    CssInside +=    "QHeaderView::section	{color: cyan; background-color: grey;}";

    CssInside +=    "QSpinBox::up-button	{width: 32px; border-width: 2px; border-style: solid; border-color:cyan; border-radius: 10px; image: url(:/resources/images/UpArrow.png);}";
    CssInside +=    "QSpinBox::down-button	{width: 32px; border-width: 2px; border-style: solid; border-color:cyan; border-radius: 10px; image: url(:/resources/images/DownArrow.png);}";
    CssInside +=    "QSpinBox			{background-color: grey;}";
    CssInside +=    "QSpinBox			{border-width: 2px; border-style: solid; border-color: cyan; border-radius: 10px;}";

    CssInside +=    "QLineEdit			{color: white; background-color: black; border: solid white; border-width: 2px; border-radius: 5px;}";

	CssInside +=    "QTreeView			{color: white; border: solid white; border-width: 2px; border-radius: 1px; padding: 5px;}";
	CssInside +=    "QTreeView::item		{height: 30px;}";
	CssInside +=    "QTreeView::item		{border-style: solid; border-color: black; border-width: 5px; border-radius: 10px;}";
	CssInside +=    "QTreeView::item		{background-color: gray;}";
	CssInside +=    "QTreeView::item:selected	{color: cyan; background-color: grey;}";
	CssInside +=    "QTreeView::item:selected	{border-top: 2px solid cyan; border-bottom: 2px solid cyan; border-left: 2px solid cyan; border-right: 2px solid cyan;}";

    //Overides for player buttons in  PlayersWindow
	CssInsideButtonSelected      = "border-color: cyan; color: cyan ;";
    CssInsideButtonUnSelected    = "color: white;";
    CssInsideButtonChecked       = "border-color: cyan; color: cyan;";
    CssInsideButtonUnChecked     = "border-color: grey; color: black;";
    CssInsideButtonPressedLast   = "color:fuchsia;";

//------------------------------------------------------------------------------------------------------------

    //CSS stylesheet for LIGHT interface
    CssOutside  =    "QWidget               {background-color: black; color: fuchsia; font: bold;}";
    CssOutside +=    "QSpinBox              {color: fuchsia;} ";
    CssOutside +=    "QMessageBox           {font: fuchsia;} ";
    CssOutside +=    "QMenu                 {font:fuchsia; color:fuchsia;}";
	CssOutside +=							qLabelFont;
	CssOutside +=    "QLabel                {color: fuchsia;}";
	//CssOutside +=    "QListWidget           {color: fuchsia;}";
    CssOutside +=    "QListWidget           {color: fuchsia; border: solid lightyellow; border-width: 2px; border-radius: 10px;}";
    CssOutside +=    "QListWidget::item	    {border-top: 2px solid lightyellow;}";
    CssOutside +=    "QListWidget::item	    {border-left: 2px solid lightyellow;}";
    CssOutside +=    "QListWidget::item	    {border-radius: 10px}";
    CssOutside +=    "QListWidget::item	    {background-color: lightyellow;}";
    CssOutside +=    "QListWidget::item	    {color: fuchsia}";

    CssOutside +=    "QToolBar              {border: lightyellow; border-width: 2px; background-color: grey; spacing: 17px; padding: 10px;}";
	CssOutside +=							qToolButtonFont;
	CssOutside +=    "QToolButton           {border-width: 2px; border-radius: 10px; border-color: cyan; background-color: lightyellow; padding: 5px; spacing: 10px;}";
    CssOutside +=    "QToolButton:checked   {background-color: yellow; border-color: fuchsia; border-color: fuchsia;}";
	CssOutside +=							qPushButtonFont;
	CssOutside +=    "QPushButton           {background-color: lightyellow; border-style: outset; border-width: 2px; border-radius: 10px; border-color: lightyellow; min-width: 50; min-height: 30;}";
    CssOutside +=    "QPushButton:disabled  {background-color: grey; border: grey; font: normal; color: black;}";
    CssOutside +=    "QPushButton:checked   {border-color: fuchsia; color: fuchsia; background-color: yellow}";
    CssOutside +=    "QSlider               {min-height: 35px; background: lightyellow; border-style: outset; border-width: 1px; border-radius: 10px; border-color: black;}";
    CssOutside +=    "QSlider::groove:horizontal {border-style: outset; border-width: 2px; border-radius: 10px; border-color: black;}";
    CssOutside +=    "QSlider::handle:horizontal {background: fuchsia; border: 1px solid #5c5c5c; width: 18px; margin: -2px 0;";
    CssOutside +=                                "border-style: outset; border-width: 2px; border-radius: 5px; border-color: fuchsia;}";
    CssOutside +=    "QSlider:disabled           {background-color: rgb(50, 50, 50);}";
    CssOutside +=    "QSlider::handle:disabled   {border-color: grey;}";
    CssOutside +=    "QTableWidget		{color: fuchsia; alternate-background-color: lightyellow; background-color: white;}";
    CssOutside +=    "QHeaderView::section	{color: fuchsia; background-color: yellow;}";
    CssOutside +=    "QSpinBox::up-button	{width: 32px; border-width: 2px; border-style: solid; border-color:fuchsia; border-radius: 10px; image: url(:/resources/images/UpArrow.png);}";
    CssOutside +=    "QSpinBox::down-button	{width: 32px; border-width: 2px; border-style: solid; border-color:fuchsia; border-radius: 10px; image: url(:/resources/images/DownArrow.png);}";
    CssOutside +=    "QSpinBox			{background-color: lightyellow;}";
    CssOutside +=    "QSpinBox			{border-width: 2px; border-style: solid; border-color: lightyellow; border-radius: 10px;}";
    CssOutside +=    "QLineEdit			{color: fuchsia; background-color: lightyellow; border: solid fuchsia; border-width: 2px; border-radius: 5px;}";
    CssOutside +=    "QTreeView		    {color: fuchsia; border: solid fuchsia; border-width: 2px; border-radius: 1px; padding: 5px;}";
    CssOutside +=    "QTreeView::item	    {height: 30px;}";
    CssOutside +=    "QTreeView::item	    {border-style: solid; border-color: black; border-width: 5px; border-radius: 10px;}";
    CssOutside +=    "QTreeView::item	    {background-color: lightyellow;}";
    CssOutside +=    "QTreeView::item:selected    {color: fuchsia;background-color: yellow;}";
    CssOutside +=    "QTreeView::item:selected    {border-top: 2px solid fuchsia; border-bottom: 2px solid fuchsia; border-left: 2px solid fuchsia; border-right: 2px solid fuchsia;}";

    //Overides for player buttons in  PlayersWindow
    CssOutsideButtonSelected      = "border-color: fuchsia; color: fuchsia;";
    CssOutsideButtonUnSelected    = "border-color: lightyellow; color: fuchsia;";
    CssOutsideButtonChecked       = "border-color: fuchsia; color: fuchsia; background-color: yellow";
    CssOutsideButtonUnChecked     = "border-color: lightyellow; color: fuchsia;";
    CssOutsideButtonPressedLast   = "color:blue;";
}

QString StyleSheet::getYellowButtonCss() const
{
    return YellowButtonCss;
}

QString StyleSheet::getGreenButtonCss() const
{
    return GreenButtonCss;
}

void StyleSheet::setSpecials(int value)
{
    if(value == CssDark)
    {
		ButtonSelectedCss      = CssInsideButtonSelected;
        ButtonUnSelectedCss    = CssInsideButtonSelected;
        ButtonCheckedCss       = CssInsideButtonChecked;
        ButtonUnCheckedCss     = CssInsideButtonUnChecked;
        ButtonPressedLastCss   = CssInsideButtonPressedLast;
    }
    else if (value == CssLight)
    {
        ButtonSelectedCss      = CssOutsideButtonSelected;
        ButtonUnSelectedCss    = CssOutsideButtonSelected;
        ButtonCheckedCss       = CssOutsideButtonChecked;
        ButtonUnCheckedCss     = CssOutsideButtonUnChecked;
        ButtonPressedLastCss   = CssOutsideButtonPressedLast;
	}
}

void StyleSheet::setFontSizes()
{
	//TODO: Calculate screen DPI and use to set Pixels.
	fontSizeforButtons	= 24;
	fontSizeForLabels	= fontSizeforButtons * .85;

	qToolButtonFont = QString::number(fontSizeforButtons);
	qToolButtonFont.prepend("QToolButton {font-size: ");
	qToolButtonFont.append("px;}");

	qPushButtonFont = QString::number(fontSizeforButtons);
	qPushButtonFont.prepend("QPushButton {font-size: ");
	qPushButtonFont.append("px;}");

	qLabelFont = QString::number(fontSizeForLabels);
	qLabelFont.prepend("QLabel {font-size: ");
	qLabelFont.append("px;}");
}


