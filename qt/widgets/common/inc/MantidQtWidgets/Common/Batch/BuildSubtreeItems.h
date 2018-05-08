#ifndef MANTIDQTMANTIDWIDGETS_BUILDSUBTREE_H_
#define MANTIDQTMANTIDWIDGETS_BUILDSUBTREE_H_
#include "MantidQtWidgets/Common/Batch/AssertOrThrow.h"
#include "MantidQtWidgets/Common/DllOption.h"
#include "MantidQtWidgets/Common/Batch/Row.h"
#include "MantidQtWidgets/Common/Batch/QtStandardItemTreeAdapter.h"
#include <QStandardItem>

namespace MantidQt {
namespace MantidWidgets {
namespace Batch {

class EXPORT_OPT_MANTIDQT_COMMON BuildSubtreeItems {
public:
  using SubtreeConstIterator = typename Subtree::const_iterator;

  void operator()(QStandardItem *parentOfSubtreeRootItem,
                  RowLocation const &parentOfSubtreeRoot, int index,
                  Subtree const &subtree);

  SubtreeConstIterator buildRecursively(QStandardItem *parentItem, int index,
                                        RowLocation const &parent, int depth,
                                        SubtreeConstIterator current,
                                        SubtreeConstIterator end);
};

} // namespace Batch
} // namespace MantidWidgets
} // namespace MantidQt
#endif