#ifndef __BOOLOPTION_H__
#define __BOOLOPTION_H__

#include <qstring.h>
#include <qcheckbox.h>
#include "option.h"

class BoolOption : public Option {
 Q_OBJECT
public:
 BoolOption(const QString &_key=0,QWidget *parent=0);
 virtual ~BoolOption();
 virtual QSize sizeHint() const;
 virtual void writeValue();
 virtual void readValue();
protected:
 virtual void resizeEvent (QResizeEvent *e);
protected:
 /** checkbox used for editing the value */
 QCheckBox *ed;
};

#endif
