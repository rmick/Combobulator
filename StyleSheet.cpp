#include "StyleSheet.h"

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
    //Combobulator Status Indicator LED (QPushbutton) in HostGame window.
    YellowButtonCss =  "background-color: yellow; border-style: outset; border-width: 2px; border-radius: 10px; border-color: grey; min-width: 0; min-height: 0";
    GreenButtonCss  =  "background-color: rgb(0,255,50); border-style: outset; border-width: 2px; border-radius: 10px; border-color: grey; min-width: 0; min-height: 0";

    //CSS stylesheet for DARK interface
    CssInside  =    "QWidget                {background-color: black; color: white;}";
    CssInside +=    "QSpinBox               {color: white;} ";
    CssInside +=    "QMessageBox            {font: white;} ";
    CssInside +=    "QMenu                  {font:white; color:white;}";
    CssInside +=    "QLabel                 {color: white;}";
    CssInside +=    "QListWidget            {color: white;}";
    CssInside +=    "QToolBar               {border: grey; border-width: 2px; background-color: rgb(50,50,50); spacing: 20px; padding: 10px;}";
    CssInside +=    "QToolButton            {border-width: 2px; border-radius: 10px; border-color: cyan; background-color: grey; padding: 5px; spacing: 20px;}";
    CssInside +=    "QToolButton:checked    {color: cyan; border-color: cyan;}";
    CssInside +=    "QPushButton            {background-color: grey; border-style: outset; border-width: 2px; border-radius: 10px; border-color: grey; min-width: 50; min-height: 30;}";
    CssInside +=    "QPushButton:disabled   {background-color: rgb(50, 50, 50);}";
    CssInside +=    "QPushButton:checked    {border-color: cyan; color: cyan;}";
    CssInside +=    "QSlider                {min-height: 35px; background: grey; border-style: outset; border-width: 1px; border-radius: 10px; border-color: black;}";
    CssInside +=    "QSlider::groove:horizontal {border-style: outset; border-width: 2px; border-radius: 10px; border-color: black;}";
    CssInside +=    "QSlider::handle:horizontal {background: grey; border: 1px solid #5c5c5c; width: 18px; margin: -2px 0;";
    CssInside +=                                "border-style: outset; border-width: 2px; border-radius: 5px; border-color: cyan;}";
    CssInside +=    "QSlider:disabled           {background-color: rgb(50, 50, 50);}";
    CssInside +=    "QSlider::handle:disabled   {border-color: grey;}";
    //Overides for player buttons in  PlayersWindow
    CssInsideButtonSelected      = "border-color: cyan; color: cyan;";
    CssInsideButtonUnSelected    = "color: white;";
    CssInsideButtonChecked       = "border-color: cyan; color: cyan;";
    CssInsideButtonUnChecked     = "border-color: grey; color: white;";
    CssInsideButtonPressedLast   = "color:fuchsia;";

    //CSS stylesheet for LIGHT interface
    CssOutside  =    "QWidget               {background-color: white; color: fuchsia; font: bold;}";
    CssOutside +=    "QSpinBox              {color: fuchsia;} ";
    CssOutside +=    "QMessageBox           {font: fuchsia;} ";
    CssOutside +=    "QMenu                 {font:fuchsia; color:fuchsia;}";
    CssOutside +=    "QLabel                {color: fuchsia;}";
    CssOutside +=    "QListWidget           {color: fuchsia;}";
    CssOutside +=    "QToolBar              {border: lightyellow; border-width: 2px; background-color: white; spacing: 20px; padding: 10px;}";
    CssOutside +=    "QToolButton           {border-width: 2px; border-radius: 10px; border-color: cyan; background-color: grey; padding: 5px; spacing: 20px;}";
    CssOutside +=    "QToolButton:checked   {color: cyan; border-color: cyan;}";
    CssOutside +=    "QPushButton           {background-color: lightyellow; border-style: outset; border-width: 2px; border-radius: 10px; border-color: lightyellow; min-width: 50; min-height: 30;}";
    CssOutside +=    "QPushButton:disabled  {background-color: white;}";
    CssOutside +=    "QPushButton:checked   {border-color: fuchsia; color: fuchsia; background-color: yellow}";
    CssOutside +=    "QSlider               {min-height: 35px; background: grey; border-style: outset; border-width: 1px; border-radius: 10px; border-color: black;}";
    CssOutside +=    "QSlider::groove:horizontal {border-style: outset; border-width: 2px; border-radius: 10px; border-color: black;}";
    CssOutside +=    "QSlider::handle:horizontal {background: grey; border: 1px solid #5c5c5c; width: 18px; margin: -2px 0;";
    CssOutside +=                                "border-style: outset; border-width: 2px; border-radius: 5px; border-color: cyan;}";
    CssOutside +=    "QSlider:disabled           {background-color: rgb(50, 50, 50);}";
    CssOutside +=    "QSlider::handle:disabled   {border-color: grey;}";
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


