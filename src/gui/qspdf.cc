/** @file
 QObject wrapper around CPdf (PDF document)<br>
 Export some functions to scripting.<br>
 See doc/user/scripting.xml or kernel documentation for more informations about these functions
 @author Martin Petricek
*/

#include "qspdf.h"
#include <stdio.h>

namespace gui {

/** Construct wrapper with given PDF document */
QSPdf::QSPdf(CPdf *_pdf) : QSCObject ("Pdf") {
 obj=_pdf;
}

/** destructor */
QSPdf::~QSPdf() {
}

/** Save copy under different name. Does not check if the file exists, overwrite anything without warning
 @return true if saved successfully, falsi if any error occured
*/
bool QSPdf::saveAs(QString name) {
 if (name.isNull()) return false;//No empty names!
 FILE *f=fopen(name,"wb");
 if (!f) return false; ///failed to open file
 try {
  obj->clone(f);
 } catch (...) {
  fclose(f);
  return false;
 }
 return true;
}

/** Call CPdf::getDictionary */
QSDict* QSPdf::getDictionary() {
 return new QSDict(obj->getDictionary());
}

/** Call CPdf::insertPage(page,position) */
QSPage* QSPdf::insertPage(QSPage* page, int position) {
 return new QSPage(obj->insertPage(page->get(),position));
}
/** Call CPdf::removePage(page,position) */
void QSPdf::removePage(int position) {
 obj->removePage(position);
}

/** Call CPdf::getPagePosition(page) */
int QSPdf::getPagePosition(QSPage *page) {
 return obj->getPagePosition(page->get());
}

/** Call CPdf::getPageCount() */
unsigned int QSPdf::getPageCount() {
 return obj->getPageCount();
}

/** Call CPdf::getPage(position) */
QSPage* QSPdf::getPage(int position) {
 return new QSPage(obj->getPage(position));
}

/** Call CPdf::getFirstPage() */
QSPage* QSPdf::getFirstPage() {
 return new QSPage(obj->getFirstPage());
}

/** Call CPdf::getNextPage(page) */
QSPage* QSPdf::getNextPage(QSPage* page) {
 return new QSPage(obj->getNextPage(page->get()));
}

/** Call CPdf::getPrevPage(page) */
QSPage* QSPdf::getPrevPage(QSPage* page) {
 return new QSPage(obj->getPrevPage(page->get()));
}

/** Call CPdf::hasNextPage(page) */
bool QSPdf::hasNextPage(QSPage* page) {
 return obj->hasNextPage(page->get());
}

/** Call CPdf::hasPrevPage(page) */
bool QSPdf::hasPrevPage(QSPage* page) {
 return obj->hasPrevPage(page->get());
}

/** Call CPdf::getRevisionsCount() */
size_t QSPdf::getRevisionsCount() {
 return obj->getRevisionsCount();
}

/** Call CPdf::getLastPage() */
QSPage* QSPdf::getLastPage() {
 return new QSPage(obj->getLastPage());
}

/** get CPdf held inside this class. Not exposed to scripting */
CPdf* QSPdf::get() {
 return obj;
}

} // namespace gui 

//TODO: implement outlines and revisions, rest of functions ...
