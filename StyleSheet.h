#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <QString>

class StyleSheet
{

private:
    int         currentCSS;

    QString     CssInside;
    QString     CssOutside;
    QString     YellowButtonCss;
    QString     GreenButtonCss;
    QString     ButtonSelectedCss;
    QString     ButtonUnSelectedCss;
    QString     ButtonCheckedCss;
    QString     ButtonUnCheckedCss;
    QString     ButtonPressedLastCss;

    QString     CssOutsideButtonSelected;
    QString     CssOutsideButtonUnSelected;
    QString     CssOutsideButtonChecked;
    QString     CssOutsideButtonUnChecked;
    QString     CssOutsideButtonPressedLast;

    QString     CssInsideButtonSelected;
    QString     CssInsideButtonUnSelected;
    QString     CssInsideButtonChecked;
    QString     CssInsideButtonUnChecked;
    QString     CssInsideButtonPressedLast;

	int			fontSizeForLabels;
	int			fontSizeforButtons;
	QString		qLabelFont;
	QString		qToolButtonFont;
	QString		qPushButtonFont;


    void        setSpecials(int value);
	void		setFontSizes();

public:
    StyleSheet();

    //used by whoever calls to set the mode
    const int   CssLight = 1;
    const int   CssDark  = 2;

    QString getButtonSelectedCss() const;
    QString getButtonUnSelectedCss() const;
    QString getButtonCheckedCss() const;
    QString getButtonUnCheckedCss() const;
    QString getButtonPressedLastCss() const;
    QString getCurrentCSSstring() const;
    int     getCurrentCSS() const;
	void	setCurrentCSS(int value);
    QString getYellowButtonCss() const;
    QString getGreenButtonCss() const;
};

extern StyleSheet myStyleSheet;

#endif // STYLESHEET_H
