#pragma once

#include <QString>
#include <wobjectdefs.h>
#include <QWidget>
#include <Scenario/Inspector/ExpressionValidator.hpp>
#include <State/Expression.hpp>

class QComboBox;
class QLabel;
class QLineEdit;
class QToolButton;
class QMenu;
class QPushButton;
namespace Device
{
class AddressAccessorEditWidget;
}

namespace Scenario
{

enum ExpressionEditorComparator
{
  // Same as in Relation
  Equal,
  Different,
  Greater,
  Lower,
  GreaterEqual,
  LowerEqual,

  // Additional ones
  Pulse,
  AlwaysTrue,
  AlwaysFalse
};

inline const std::map<ExpressionEditorComparator, QString>&
ExpressionEditorComparators();

// TODO move in plugin state
class SimpleExpressionEditorWidget final : public QWidget
{
  W_OBJECT(SimpleExpressionEditorWidget)
public:
  SimpleExpressionEditorWidget(
      const score::DocumentContext&,
      int index,
      QWidget* parent = nullptr,
      QMenu* menu = nullptr);

  State::Expression relation();
  optional<State::BinaryOperator> binOperator();

  int id = -1;

  void setRelation(State::Relation r);
  void setPulse(State::Pulse r);
  void setOperator(State::BinaryOperator o);
  void setOperator(State::UnaryOperator u);

  QString currentRelation();
  QString currentOperator();

  void enableRemoveButton(bool);
  void enableAddButton(bool);
  void enableMenuButton(bool);

public:
  void editingFinished() W_SIGNAL(editingFinished);
  void addTerm() W_SIGNAL(addTerm);
  void removeTerm(int index) W_SIGNAL(removeTerm, index);

private:
  void on_editFinished();
  void on_comparatorChanged(int i);
  // TODO on_modelChanged() -> done in parent inspector (i.e. event), no ?

  QLabel* m_ok{};

  Device::AddressAccessorEditWidget* m_address{};
  QComboBox* m_comparator{};
  QLineEdit* m_value{};
  QComboBox* m_binOperator{};
  QToolButton* m_rmBtn{};
  QToolButton* m_addBtn{};
  QPushButton* m_menuBtn{};

  ExpressionValidator<State::Expression> m_validator;
  QString m_relation{};
  QString m_op{};
};
}

Q_DECLARE_METATYPE(Scenario::ExpressionEditorComparator)
W_REGISTER_ARGTYPE(Scenario::ExpressionEditorComparator)
