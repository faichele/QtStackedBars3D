#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QFontComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtGui/QScreen>
#include <QtGui/QFontDatabase>

#include "GraphModifier.h"
#include "GraphModifierStacked.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QBars3D *widgetgraph = new QBars3D();
	QWidget *container = QWidget::createWindowContainer(widgetgraph);

	QStackedBars3D* stackedGraph = new QStackedBars3D();
	QWidget* stackedContainer = QWidget::createWindowContainer(stackedGraph);

	if (!widgetgraph->hasContext()) {
		QMessageBox msgBox;
		msgBox.setText("Couldn't initialize the OpenGL context.");
		msgBox.exec();
		return -1;
	}

	if (!stackedGraph->hasContext()) {
		QMessageBox msgBox;
		msgBox.setText("Couldn't initialize the OpenGL context.");
		msgBox.exec();
		return -1;
	}

	QSize screenSize = widgetgraph->screen()->size();
	container->setMinimumSize(QSize(screenSize.width() / 3, screenSize.height() / 1.75));
	container->setMaximumSize(QSize(screenSize.width() / 2, screenSize.height() / 2));
	container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	container->setFocusPolicy(Qt::StrongFocus);

	stackedContainer->setMinimumSize(QSize(screenSize.width() / 3, screenSize.height() / 1.75));
	stackedContainer->setMaximumSize(QSize(screenSize.width() / 2, screenSize.height() / 2));
	stackedContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	stackedContainer->setFocusPolicy(Qt::StrongFocus);

	QWidget *widget = new QWidget;
	QHBoxLayout *hLayout = new QHBoxLayout(widget);
	QVBoxLayout *vLayout = new QVBoxLayout();
	
	QVBoxLayout* boxLayout1 = new QVBoxLayout();
	QLabel* graphLabel1 = new QLabel("3D bars diagram");
	boxLayout1->addWidget(graphLabel1);
	boxLayout1->addWidget(container);

	hLayout->addLayout(boxLayout1, 1);

	QVBoxLayout* boxLayout2 = new QVBoxLayout();
	QLabel* graphLabel2 = new QLabel("3D stacked bars diagram");
	boxLayout2->addWidget(graphLabel2);
	boxLayout2->addWidget(stackedContainer);

	hLayout->addLayout(boxLayout2, 1);
	hLayout->addLayout(vLayout);

	widget->setWindowTitle(QStringLiteral("Stacked 3D bars"));

	QComboBox* controlTargetList = new QComboBox(widget);
	controlTargetList->addItem(QStringLiteral("3D bars diagram"));
	controlTargetList->addItem(QStringLiteral("3D stacked bars diagram"));
	controlTargetList->setCurrentIndex(1);

	QComboBox *themeList = new QComboBox(widget);
	themeList->addItem(QStringLiteral("Qt"));
	themeList->addItem(QStringLiteral("Primary Colors"));
	themeList->addItem(QStringLiteral("Digia"));
	themeList->addItem(QStringLiteral("Stone Moss"));
	themeList->addItem(QStringLiteral("Army Blue"));
	themeList->addItem(QStringLiteral("Retro"));
	themeList->addItem(QStringLiteral("Ebony"));
	themeList->addItem(QStringLiteral("Isabelle"));
	themeList->setCurrentIndex(0);

	QPushButton *labelButton = new QPushButton(widget);
	labelButton->setText(QStringLiteral("Change label style"));

	QCheckBox *smoothCheckBox = new QCheckBox(widget);
	smoothCheckBox->setText(QStringLiteral("Smooth bars"));
	smoothCheckBox->setChecked(false);

	QComboBox *barStyleList = new QComboBox(widget);
	barStyleList->addItem(QStringLiteral("Bar"), int(QAbstractSeries3D::MeshBar));
	barStyleList->addItem(QStringLiteral("Pyramid"), int(QAbstractSeries3D::MeshPyramid));
	barStyleList->addItem(QStringLiteral("Cone"), int(QAbstractSeries3D::MeshCone));
	barStyleList->addItem(QStringLiteral("Cylinder"), int(QAbstractSeries3D::MeshCylinder));
	barStyleList->addItem(QStringLiteral("Bevel bar"), int(QAbstractSeries3D::MeshBevelBar));
	barStyleList->addItem(QStringLiteral("Sphere"), int(QAbstractSeries3D::MeshSphere));
	barStyleList->setCurrentIndex(4);

	QPushButton *cameraButton = new QPushButton(widget);
	cameraButton->setText(QStringLiteral("Change camera preset"));

	QPushButton *zoomToSelectedButton = new QPushButton(widget);
	zoomToSelectedButton->setText(QStringLiteral("Zoom to selected bar"));

	QComboBox *selectionModeList = new QComboBox(widget);
	selectionModeList->addItem(QStringLiteral("None"),
		int(QAbstractGraph3D::SelectionNone));
	selectionModeList->addItem(QStringLiteral("Bar"),
		int(QAbstractGraph3D::SelectionItem));
	selectionModeList->addItem(QStringLiteral("Row"),
		int(QAbstractGraph3D::SelectionRow));
	selectionModeList->addItem(QStringLiteral("Bar and Row"),
		int(QAbstractGraph3D::SelectionItemAndRow));
	selectionModeList->addItem(QStringLiteral("Column"),
		int(QAbstractGraph3D::SelectionColumn));
	selectionModeList->addItem(QStringLiteral("Bar and Column"),
		int(QAbstractGraph3D::SelectionItemAndColumn));
	selectionModeList->addItem(QStringLiteral("Row and Column"),
		int(QAbstractGraph3D::SelectionRowAndColumn));
	selectionModeList->addItem(QStringLiteral("Bar, Row and Column"),
		int(QAbstractGraph3D::SelectionItemRowAndColumn));
	selectionModeList->addItem(QStringLiteral("Slice into Row"),
		int(QAbstractGraph3D::SelectionSlice | QAbstractGraph3D::SelectionRow));
	selectionModeList->addItem(QStringLiteral("Slice into Row and Item"),
		int(QAbstractGraph3D::SelectionSlice | QAbstractGraph3D::SelectionItemAndRow));
	selectionModeList->addItem(QStringLiteral("Slice into Column"),
		int(QAbstractGraph3D::SelectionSlice | QAbstractGraph3D::SelectionColumn));
	selectionModeList->addItem(QStringLiteral("Slice into Column and Item"),
		int(QAbstractGraph3D::SelectionSlice | QAbstractGraph3D::SelectionItemAndColumn));
	selectionModeList->addItem(QStringLiteral("Multi: Bar, Row, Col"),
		int(QAbstractGraph3D::SelectionItemRowAndColumn
			| QAbstractGraph3D::SelectionMultiSeries));
	selectionModeList->addItem(QStringLiteral("Multi, Slice: Row, Item"),
		int(QAbstractGraph3D::SelectionSlice | QAbstractGraph3D::SelectionItemAndRow
			| QAbstractGraph3D::SelectionMultiSeries));
	selectionModeList->addItem(QStringLiteral("Multi, Slice: Col, Item"),
		int(QAbstractGraph3D::SelectionSlice | QAbstractGraph3D::SelectionItemAndColumn
			| QAbstractGraph3D::SelectionMultiSeries));
	selectionModeList->setCurrentIndex(1);

	QCheckBox *backgroundCheckBox = new QCheckBox(widget);
	backgroundCheckBox->setText(QStringLiteral("Show background"));
	backgroundCheckBox->setChecked(false);

	QCheckBox *gridCheckBox = new QCheckBox(widget);
	gridCheckBox->setText(QStringLiteral("Show grid"));
	gridCheckBox->setChecked(true);

	QCheckBox *seriesCheckBox = new QCheckBox(widget);
	seriesCheckBox->setText(QStringLiteral("Show second series"));
	seriesCheckBox->setChecked(false);

	QCheckBox *reverseValueAxisCheckBox = new QCheckBox(widget);
	reverseValueAxisCheckBox->setText(QStringLiteral("Reverse value axis"));
	reverseValueAxisCheckBox->setChecked(false);

	QCheckBox *reflectionCheckBox = new QCheckBox(widget);
	reflectionCheckBox->setText(QStringLiteral("Show reflections"));
	reflectionCheckBox->setChecked(false);

	QSlider *rotationSliderX = new QSlider(Qt::Horizontal, widget);
	rotationSliderX->setTickInterval(30);
	rotationSliderX->setTickPosition(QSlider::TicksBelow);
	rotationSliderX->setMinimum(-180);
	rotationSliderX->setValue(0);
	rotationSliderX->setMaximum(180);
	QSlider *rotationSliderY = new QSlider(Qt::Horizontal, widget);
	rotationSliderY->setTickInterval(15);
	rotationSliderY->setTickPosition(QSlider::TicksAbove);
	rotationSliderY->setMinimum(-90);
	rotationSliderY->setValue(0);
	rotationSliderY->setMaximum(90);

	QSlider *fontSizeSlider = new QSlider(Qt::Horizontal, widget);
	fontSizeSlider->setTickInterval(10);
	fontSizeSlider->setTickPosition(QSlider::TicksBelow);
	fontSizeSlider->setMinimum(1);
	fontSizeSlider->setValue(30);
	fontSizeSlider->setMaximum(100);

	QFontComboBox *fontList = new QFontComboBox(widget);
	fontList->setCurrentFont(QFont("Times New Roman"));

	QComboBox *shadowQuality = new QComboBox(widget);
	shadowQuality->addItem(QStringLiteral("None"));
	shadowQuality->addItem(QStringLiteral("Low"));
	shadowQuality->addItem(QStringLiteral("Medium"));
	shadowQuality->addItem(QStringLiteral("High"));
	shadowQuality->addItem(QStringLiteral("Low Soft"));
	shadowQuality->addItem(QStringLiteral("Medium Soft"));
	shadowQuality->addItem(QStringLiteral("High Soft"));
	shadowQuality->setCurrentIndex(5);

	QComboBox *rangeList = new QComboBox(widget);

	QCheckBox *axisTitlesVisibleCB = new QCheckBox(widget);
	axisTitlesVisibleCB->setText(QStringLiteral("Axis titles visible"));
	axisTitlesVisibleCB->setChecked(true);

	QCheckBox *axisTitlesFixedCB = new QCheckBox(widget);
	axisTitlesFixedCB->setText(QStringLiteral("Axis titles fixed"));
	axisTitlesFixedCB->setChecked(true);

	QSlider *axisLabelRotationSlider = new QSlider(Qt::Horizontal, widget);
	axisLabelRotationSlider->setTickInterval(10);
	axisLabelRotationSlider->setTickPosition(QSlider::TicksBelow);
	axisLabelRotationSlider->setMinimum(0);
	axisLabelRotationSlider->setValue(30);
	axisLabelRotationSlider->setMaximum(90);

	vLayout->addWidget(new QLabel(QStringLiteral("Target graph for control")));
	vLayout->addWidget(controlTargetList);
	vLayout->addWidget(new QLabel(QStringLiteral("Rotate horizontally")));
	vLayout->addWidget(rotationSliderX, 0, Qt::AlignTop);
	vLayout->addWidget(new QLabel(QStringLiteral("Rotate vertically")));
	vLayout->addWidget(rotationSliderY, 0, Qt::AlignTop);
	vLayout->addWidget(labelButton, 0, Qt::AlignTop);
	vLayout->addWidget(cameraButton, 0, Qt::AlignTop);
	vLayout->addWidget(zoomToSelectedButton, 0, Qt::AlignTop);
	vLayout->addWidget(backgroundCheckBox);
	vLayout->addWidget(gridCheckBox);
	vLayout->addWidget(smoothCheckBox);
	vLayout->addWidget(reflectionCheckBox);
	vLayout->addWidget(seriesCheckBox);
	vLayout->addWidget(reverseValueAxisCheckBox);
	vLayout->addWidget(axisTitlesVisibleCB);
	vLayout->addWidget(axisTitlesFixedCB);
	vLayout->addWidget(new QLabel(QStringLiteral("Show cycle")));
	vLayout->addWidget(rangeList);
	vLayout->addWidget(new QLabel(QStringLiteral("Change bar style")));
	vLayout->addWidget(barStyleList);
	vLayout->addWidget(new QLabel(QStringLiteral("Change selection mode")));
	vLayout->addWidget(selectionModeList);
	vLayout->addWidget(new QLabel(QStringLiteral("Change theme")));
	vLayout->addWidget(themeList);
	vLayout->addWidget(new QLabel(QStringLiteral("Adjust shadow quality")));
	vLayout->addWidget(shadowQuality);
	vLayout->addWidget(new QLabel(QStringLiteral("Change font")));
	vLayout->addWidget(fontList);
	vLayout->addWidget(new QLabel(QStringLiteral("Adjust font size")));
	vLayout->addWidget(fontSizeSlider);
	vLayout->addWidget(new QLabel(QStringLiteral("Axis label rotation")));
	vLayout->addWidget(axisLabelRotationSlider, 1, Qt::AlignTop);

	GraphModifier *modifier = new GraphModifier(widgetgraph);
	GraphModifierStacked* stackedModifier = new GraphModifierStacked(stackedGraph);

	CurrentControlTarget* controlTarget = new CurrentControlTarget(modifier, stackedModifier,
		rotationSliderX, rotationSliderY, labelButton, cameraButton, zoomToSelectedButton,
		backgroundCheckBox, gridCheckBox, seriesCheckBox, reverseValueAxisCheckBox, reflectionCheckBox, smoothCheckBox,
		rangeList, fontList, selectionModeList, barStyleList, themeList, shadowQuality,
		fontSizeSlider, axisLabelRotationSlider, axisTitlesVisibleCB, axisTitlesFixedCB);

	for (int k = 0; k < modifier->getCycleCount(); k++)
	{
		rangeList->addItem(QString("Takt ") + QString::number(k + 1));
	}
	rangeList->addItem(QStringLiteral("All"));
	rangeList->setCurrentIndex(modifier->getCycleCount());

	QObject::connect(controlTargetList, SIGNAL(currentIndexChanged(int)), controlTarget, SLOT(controlTargetChanged(int)));

	controlTarget->switchTarget();
	
	widget->show();

	return app.exec();
}