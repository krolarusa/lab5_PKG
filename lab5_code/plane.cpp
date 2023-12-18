#include <QVector>
#include <QPair>
#include "plane.h"
#include <fstream>
#include <iostream>
#include <cmath>

plane::plane(type t, QWidget *parent): QWidget(parent)
{
    // Открываем файл для чтения
    std::ifstream fin("C:/cpp/projectsQt/lab5_code/input.txt");

    // Считываем количество отрезков
    int n;
    fin >> n;

    int x1, y1, x2, y2;
    QPointF p1, p2;

    // Считываем отрезки и сохраняем в структуру данных
    for (int i = 0; i < n; i++) {
        fin >> x1 >> y1 >> x2 >> y2;
        p1.setX(x1);
        p1.setY(y1);
        p2.setX(x2);
        p2.setY(y2);
        segments.push_back(qMakePair(p1, p2));
    }

    // Считываем границы прямоугольной области отсечения
    fin >> Xmin >> Ymin >> Xmax >> Ymax;

    // Считываем количество вершин полигона
    fin >> n;

    // Считываем вершины полигона и сохраняем в структуру данных
    for (int i = 0; i < n; i++) {
        fin >> x1 >> y1 >> x2 >> y2;
        p1.setX(x1);
        p1.setY(y1);
        p2.setX(x2);
        p2.setY(y2);
        polygon.push_back(qMakePair(p1, p2));
    }

    // Сохраняем тип алгоритма
    this->t = t;

    // В зависимости от типа алгоритма вызываем соответствующую функцию
    if (t == type::Central) {
        clipSegments(); // Отсечение отрезков
    } else {
        Cirus(); // алгоритм отсечения отрезков
    }
}

// Функция для отсечения отрезков
void plane::clipSegments() {
    QVector<QPair<QPointF, QPointF>> segments = this->segments;
    for (int i = 0; i < segments.size(); i++) {
        std::cout << i << std::endl;
        QPointF P1 = segments[i].first;
        QPointF P2 = segments[i].second;

        // Получаем коды концевых точек отрезка
        int P1_code = getCode(P1), P2_code = getCode(P2);

        // Если отрезок находится внутри области отсечения, добавляем его в результирующий набор
        if (sqrt(pow(P1.x() - P2.x(), 2) + pow(P1.y() - P2.y(), 2)) <= 1) {
            return;
        }

        if ((P1_code | P2_code) == 0) {
            clippedSegments.push_back(segments[i]);
        } else if ((P1_code & P2_code) != 0) {
            continue;
        } else {
            if (P1_code == 0) {
                QPointF tmp = P1;
                P1 = P2;
                P2 = tmp;
            }
            // Вычисляем новую конечную точку отрезка и заменяем текущий отрезок в наборе данных
            segments[i] = qMakePair(P2, intersectionPoint(P1, P2));
            i--;
        }
    }
}

// Получение кода точки
int plane::getCode(QPointF point) {
    double x = point.x();
    double y = point.y();
    int code = 0;

    if (x < Xmin)
        code += 1;
    if (x > Xmax)
        code += 2;
    if (y < Ymin)
        code += 4;
    if (y > Ymax)
        code += 8;

    return code;
}

// Вычисление точки пересечения отрезка и границы области отсечения
QPointF plane::intersectionPoint(QPointF P1, QPointF P2) {
    int P1_code = getCode(P1);

    QPointF intersectionPoint;
    if ((P1_code & 8))
        intersectionPoint = QPointF(P1.x() + (P2.x() - P1.x()) * (Ymax - P1.y()) / (P2.y() - P1.y()),Ymax);
    else if ((P1_code & 4))
        intersectionPoint = QPointF(P1.x() + (P2.x()- P1.x()) * (Ymin - P1.y()) / (P2.y() - P1.y()),Ymin);
    else if ((P1_code & 2))
        intersectionPoint = QPointF(Xmax,P1.y() + (P2.y()- P1.y()) * (Xmax - P1.x()) / (P2.x() - P1.x()));
    else if ((P1_code & 1))
        intersectionPoint = QPointF(Xmin,P1.y() + (P2.y()- P1.y()) * (Xmin - P1.x()) / (P2.x() - P1.x()));
    return intersectionPoint;
}

void plane::Cirus(){
    for(int i = 0; i < segments.size(); i++){
        // Отсекаем текущий сегмент относительно полигона
        ClipByCirus(segments[i]);

        // Если сегмент полностью отсекается, пропускаем его
        if (t_1 < 0 || t_1 > 1 || t_2 < 0 || t_2 > 1){
            continue;
        } else {
            // Находим точки отсечения и добавляем их в результат
            QPointF begin = segments[i].first;
            QPointF end = segments[i].second;
            QPointF p1;
            p1.setX(begin.x() + t_1 * (end.x() - begin.x()));
            p1.setY(begin.y() + t_1 * (end.y() - begin.y()));
            QPointF p2;
            p2.setX(begin.x() + t_2 * (end.x() - begin.x()));
            p2.setY(begin.y() + t_2 * (end.y() - begin.y()));
            clippedSegments.push_back(qMakePair(p1, p2));
        }
    }
}

// Функция для вычисления параметра t для точки пересечения
float plane::getT(QPair<QPointF,QPointF> edge, QPair<QPointF,QPointF> segment, bool* onLine){
    bool a;
    // Вычисляем угловые коэффициенты для сегмента и ребра
    float ks = (segment.second.y()-segment.first.y())/(segment.second.x()-segment.first.x());
    float ke = (edge.second.y()-edge.first.y())/(edge.second.x()-edge.first.x());
    float bs = segment.first.y() - ks*segment.first.x();
    float be = edge.first.y() - ke*edge.first.x();
    float x = (be-bs)/(ks-ke);

    // Если точка пересечения находится вне отрезка, возвращаем -1
    if ((x - edge.first.x()) / (edge.second.x() - edge.first.x()) <= 0 || (x - edge.first.x()) / (edge.second.x() - edge.first.x()) >= 1){
        return -1;
    }

    // Если отрезок сегментирован вертикально и совпадает с ребром, помечаем его как онлайн
    if ((segment.second.x() - segment.first.x()) == 0 && ke == ks && be == bs){
        a = true;
        onLine = &a;  // Передача значения onLine обратно в вызывающую функцию
        return -1;
    } else {
        float te = (x - segment.first.x()) / (segment.second.x() - segment.first.x());
        return te;
    }
}

// Функция для вычисления скалярного произведения векторов
float ScalarMultiply(QPair<QPointF,QPointF> v1, QPair<QPointF,QPointF> v2){
    float v1x1 = v1.first.y();
    float v1x2 = v1.second.y();
    float v1y1 = v1.first.x();
    float v1y2 = v1.second.x();
    float v2x1 = v2.first.x();
    float v2x2 = v2.second.x();
    float v2y1 = v2.first.y();
    float v2y2 = v2.second.y();
    return (-(v1x2-v1x1)*(v2x2-v2x1) + (v1y2-v1y1)*(v2y2-v2y1));
}

// Функция для вычисления параметра точки относительно отрезка
float getParameterOfPoint(QPointF p, QPair<QPointF, QPointF> segment)
{
    return (p.x() - segment.first.x()) / (segment.second.x() - segment.first.x());
}

// Функция для отсечения сегмента относительно полигона
void plane::ClipByCirus(QPair<QPointF, QPointF> segment){
    QVector<float> T_enter;
    QVector<float> T_outer;
    float t, S;
    bool onLine = false;
    for(int i = 0; i<polygon.size();i++){
        // Получаем параметр t для точки пересечения и проверяем, лежит ли она на ребре
        t = getT(polygon[i], segment, &onLine);
        if(onLine){
            // Если точка лежит на ребре, добавляем параметры для отсечения
            T_enter.push_back(getParameterOfPoint(polygon[i].first, segment));
            T_outer.push_back(getParameterOfPoint(polygon[i].second, segment));
            T_outer.push_back(getParameterOfPoint(polygon[i].first, segment));
            T_enter.push_back(getParameterOfPoint(polygon[i].second, segment));
            onLine = false;
            continue;
        }
        // Вычисляем скалярное произведение и добавляем параметры для отсечения
        S=-ScalarMultiply(polygon[i], segment);
        if(t>=0 && t<=1){
            if(S>0){
                T_enter.push_back(t);
            }else if(S<0){
                T_outer.push_back(t);
            }else{
                T_enter.push_back(t);
                T_outer.push_back(t);
            }
        }
    }
    // Если не было точек отсечения, устанавливаем параметры t в -1 и завершаем функцию
    if(T_outer.size() == 0 && T_enter.size() == 0){
        t_1 = -1;
        t_2 = -1;
        return;
    }
    // Находим минимальные и максимальные значения параметра t для точек отсечения
    float t_enter = 0;
    float t_outer = 1;
    for (int i = 0; i < T_enter.size(); i++){
        if (t_enter < T_enter[i])
        {
            t_enter = T_enter[i];
        }
    }
    for (int i = 0; i < T_outer.size(); i++){
        if (t_outer > T_outer[i])
        {
            t_outer = T_outer[i];
        }
    }
    t_1 = t_enter;
    t_2 = t_outer;
}
