#pragma once

#include <QColor>
#include <wobjectdefs.h>
#include <QFormLayout>
#include <QLineEdit>
#include <QPixmap>
#include <QString>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <Scenario/Commands/Metadata/ChangeElementColor.hpp>
#include <Scenario/Commands/Metadata/ChangeElementComments.hpp>
#include <Scenario/Commands/Metadata/ChangeElementLabel.hpp>
#include <Scenario/Commands/Metadata/ChangeElementName.hpp>
#include <Scenario/Commands/Metadata/SetExtendedMetadata.hpp>
#include <Scenario/Inspector/CommentEdit.hpp>
#include <Scenario/Inspector/ExtendedMetadataWidget.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <score/model/IdentifiedObject.hpp>
#include <score/widgets/MarginLess.hpp>
#include <score/widgets/TextLabel.hpp>

namespace score
{
class ModelMetadata;
}
class QLabel;
class QLineEdit;
class QObject;
class QPushButton;
class QToolButton;

namespace color_widgets
{
class ColorPaletteModel;
}

namespace Scenario
{
class ExtendedMetadataWidget;
class CommentEdit;

// TODO move me in Process
class MetadataWidget final : public QWidget
{
  W_OBJECT(MetadataWidget)

public:
  explicit MetadataWidget(
      const score::ModelMetadata& metadata,
      const score::CommandStackFacade& m,
      const QObject* docObject,
      QWidget* parent = nullptr);

  ~MetadataWidget();

  QString scriptingName() const;

  template <typename T>
  void setupConnections(const T& model)
  {
    using namespace Scenario::Command;
    using namespace score::IDocument;
    connect(
        this, &MetadataWidget::scriptingNameChanged,
        [&](const QString& newName) {
          if (newName != model.metadata().getName())
          {
            if (!newName.isEmpty())
            {
              m_commandDispatcher.submitCommand(
                  new ChangeElementName<T>{model, newName});
            }
            else
            {
              m_commandDispatcher.submitCommand(new ChangeElementName<T>{
                  model,
                  QString("%1.0").arg(Metadata<PrettyName_k, T>::get())});
            }
          }
        });

    connect(this, &MetadataWidget::labelChanged, [&](const QString& newLabel) {
      if (newLabel != model.metadata().getLabel())
        m_commandDispatcher.submitCommand(
            new ChangeElementLabel<T>{model, newLabel});
    });

    connect(
        this, &MetadataWidget::commentsChanged,
        [&](const QString& newComments) {
          if (newComments != model.metadata().getComment())
            m_commandDispatcher.submitCommand(
                new ChangeElementComments<T>{model, newComments});
        });

    connect(
        this, &MetadataWidget::colorChanged, [&](score::ColorRef newColor) {
          if (newColor != model.metadata().getColor())
            m_commandDispatcher.submitCommand(
                new ChangeElementColor<T>{model, newColor});
        });

    connect(
        this, &MetadataWidget::extendedMetadataChanged,
        [&](const QVariantMap& newM) {
          if (newM != model.metadata().getExtendedMetadata())
            m_commandDispatcher.submitCommand(
                new SetExtendedMetadata<T>{model, newM});
        });
  }

  void setScriptingName(QString arg);
  void updateAsked();

public:
  void scriptingNameChanged(QString arg) W_SIGNAL(scriptingNameChanged, arg);
  void labelChanged(QString arg) W_SIGNAL(labelChanged, arg);
  void commentsChanged(QString arg) W_SIGNAL(commentsChanged, arg);
  void colorChanged(score::ColorRef arg) W_SIGNAL(colorChanged, arg);
  void extendedMetadataChanged(const QVariantMap& arg) W_SIGNAL(extendedMetadataChanged, arg);

private:
  static const constexpr int m_colorIconSize{21};

  const score::ModelMetadata& m_metadata;
  CommandDispatcher<> m_commandDispatcher;

  score::MarginLess<QVBoxLayout> m_metadataLayout;
  score::MarginLess<QHBoxLayout> m_headerLay;
  score::MarginLess<QVBoxLayout> m_btnLay;
  score::MarginLess<QHBoxLayout> m_cmtLay;
  QLabel* m_cmtLabel;
  QWidget m_descriptionWidget;
  score::MarginLess<QFormLayout> m_descriptionLay;
  QLineEdit m_scriptingNameLine;
  QLineEdit m_labelLine;
  CommentEdit m_comments;
  QToolButton m_colorButton;
  QToolButton m_cmtBtn;
  ExtendedMetadataWidget m_meta;
  QPixmap m_colorButtonPixmap{4 * m_colorIconSize / 3,
                              4 * m_colorIconSize / 3};

  bool m_cmtExpanded{false};
};
}
