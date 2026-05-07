#include "SimulationWidget.h"
#include <QPainter>
#include <QDir>
#include <QApplication>
#include <iostream>

SimulationWidget::SimulationWidget(adevs::Simulator<Output>* sim, 
                                   const std::vector<Autonode*>& nodes, 
                                   double env_length, 
                                   double env_width, 
                                   double sim_length,
                                   QWidget *parent)
    : QWidget(parent), m_sim(sim), m_nodes(nodes), 
      m_env_length(env_length), m_env_width(env_width), 
      m_sim_length(sim_length), m_scale(10.0), m_frameCount(0) 
{
    // Set white background
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setAutoFillBackground(true);
    setPalette(pal);

    // Create recording directory
    QDir().mkdir("Recording");

    // Setup simulation timer loop (approx 30 fps)
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SimulationWidget::stepSimulation);
    m_timer->start(33); 
}

SimulationWidget::~SimulationWidget() {}

void SimulationWidget::setScale(double scale) {
    m_scale = scale;
    update();
}

void SimulationWidget::stepSimulation() {
    // Advance simulation if within time limits
    if (m_sim->nextEventTime() < m_sim_length) {
        m_sim->execNextEvent();
        update(); // Trigger paintEvent
        recordFrame(); // Record the animation
    } else {
        m_timer->stop();
        std::cout << "Simulation finished. Frames saved to 'Recording' folder." << std::endl;
        QApplication::quit(); // Exit and finish recording
    }
}

void SimulationWidget::recordFrame() {
    // Saves current widget state as a PNG frame to compile into a video upon exit
    QString filename = QString("Recording/frame_%1.png").arg(m_frameCount++, 5, 10, QChar('0'));
    this->grab().save(filename);
}

void SimulationWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int centerX = width() / 2;
    int centerY = height() / 2;

    // Shift coordinate system so (0,0) is at the center
    painter.translate(centerX, centerY);
    
    // Qt Y-axis grows downwards, standard math grows upwards
    painter.scale(1, -1);

    // Draw Grid and Axes
    QPen gridPen(Qt::lightGray, 1, Qt::DotLine);
    painter.setPen(gridPen);
    
    double half_len = m_env_length / 2.0;
    double half_width = m_env_width / 2.0;
    
    // Draw vertical and horizontal grid lines
    for (double x = -half_len; x <= half_len; x += 1.0) {
        painter.drawLine(QPointF(x * m_scale, -half_width * m_scale), 
                         QPointF(x * m_scale, half_width * m_scale));
    }
    for (double y = -half_width; y <= half_width; y += 1.0) {
        painter.drawLine(QPointF(-half_len * m_scale, y * m_scale), 
                         QPointF(half_len * m_scale, y * m_scale));
    }

    // Draw main X and Y axes
    QPen axisPen(Qt::black, 2, Qt::SolidLine);
    painter.setPen(axisPen);
    painter.drawLine(QPointF(-half_len * m_scale, 0), QPointF(half_len * m_scale, 0)); // X axis
    painter.drawLine(QPointF(0, -half_width * m_scale), QPointF(0, half_width * m_scale)); // Y axis

    // Labels must be drawn un-inverted so text isn't upside down
    painter.save();
    painter.scale(1, -1);
    painter.drawText(QPointF(half_len * m_scale + 5, 5), "X");
    painter.drawText(QPointF(5, -half_width * m_scale - 5), "Y");
    painter.restore();

    // Draw Targets (Red "X"s)
    QPen targetPen(Qt::red, 2);
    painter.setPen(targetPen);
    for (Autonode* node : m_nodes) {
        // Targets are only determined once DEPLOYING is complete
        if (node != nullptr && node->getState() != DEPLOYING) {
            double tx = node->get_target_x();
            double ty = node->get_target_y();

            if (tx < 1e9 && ty < 1e9)
            {
                double scaledX = tx * m_scale;
                double scaledY = ty * m_scale;
                double offset = 3.0; // Size of the "X"
                painter.drawLine(QPointF(scaledX - offset, scaledY - offset), QPointF(scaledX + offset, scaledY + offset));
                painter.drawLine(QPointF(scaledX - offset, scaledY + offset), QPointF(scaledX + offset, scaledY - offset));
            }
        }
    }

    // Draw Nodes (Circular point-like objects)
    QPen nodePen(Qt::blue, 1);
    QBrush nodeBrush(Qt::blue);
    painter.setPen(nodePen);
    painter.setBrush(nodeBrush);
    
    double nodeRadius = 2.0;
    for (Autonode* node : m_nodes) {
        if (node != nullptr)
        {
            double nx = node->get_xpos() * m_scale;
            double ny = node->get_ypos() * m_scale;
            painter.drawEllipse(QPointF(nx, ny), nodeRadius, nodeRadius);
        }
    }
}
