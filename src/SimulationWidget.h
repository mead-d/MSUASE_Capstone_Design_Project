#ifndef SIMULATION_WIDGET_H
#define SIMULATION_WIDGET_H

#include <QWidget>
#include <QTimer>
#include <vector>
#include "adevs.h"
#include "Autonode.h"

class SimulationWidget : public QWidget {
    Q_OBJECT

public:
    // Initializes the widget with simulation data, environment bounds, and scaling
    SimulationWidget(adevs::Simulator<Output>* sim, 
                     const std::vector<Autonode*>& nodes, 
                     double env_length, 
                     double env_width, 
                     double sim_length,
                     QWidget *parent = nullptr);
    ~SimulationWidget();

    // Variable scaling modifier
    void setScale(double scale);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void stepSimulation();

private:
    adevs::Simulator<Output>* m_sim;
    std::vector<Autonode*> m_nodes;
    
    double m_env_length;
    double m_env_width;
    double m_sim_length;
    double m_scale; // Default scaling is 10
    
    QTimer* m_timer;
    int m_frameCount;
    
    // Helper function to save frames
    void recordFrame();
};

#endif // SIMULATION_WIDGET_H
