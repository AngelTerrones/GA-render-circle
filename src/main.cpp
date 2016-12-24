#include <cstdio>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <QDir>
#include <QtGui>
#include <QDebug>
#include <QSvgGenerator>
#include "chromosome.hpp"

#define N_CIRCLES 50
#define N_POPULATION 10
#define HAREM 0.4

int max_generation = -1;
QImage sourceImage;
int imgW = -1;
int imgH = -1;
Chromosome *populationA[N_POPULATION];
Chromosome *populationB[N_POPULATION];
int sizePopulationA = -1;
int sizePopulationB = -1;

// functions
QImage DrawImage(Chromosome *dna){
    QImage image(imgW, imgH, QImage::Format_RGB32);
    QPainter painter(&image);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);

    // black background
    painter.setBrush(QBrush(QColor(0, 0, 0, 255)));
    QPolygon polyclean;
    int pointsclean[] = {0, 0, 0, imgH, imgW, imgH, imgW, 0};
    polyclean.setPoints(4, pointsclean);
    painter.drawConvexPolygon(polyclean);

    // draw circles
    Circle *circles = dna->DNA();
    for(int n = 0; n < N_CIRCLES; n++){
        QColor color(circles[n].red(),
                     circles[n].green(),
                     circles[n].blue(),
                     circles[n].alpha());
        painter.setBrush(QBrush(color));
        painter.drawEllipse(circles[n].center(), circles[n].radius(), circles[n].radius());
    }
    painter.end();
    return image;
}

void DrawSVG(Chromosome *dna, const char *svgName){
    QSvgGenerator svgGen;
    svgGen.setFileName(QString(svgName));
    svgGen.setSize(QSize(imgW, imgH));
    svgGen.setViewBox(QRect(0, 0, imgW, imgH));

    QPainter painter(&svgGen);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);

    // Black background
    painter.setBrush(QBrush(QColor(0, 0, 0, 255)));
    QPolygon polyclean;
    int pointsclean[] = {0, 0, 0, imgH, imgW, imgH, imgW, 0};
    polyclean.setPoints(4, pointsclean);
    painter.drawConvexPolygon(polyclean);

    // draw circles
    Circle *circles = dna->DNA();
    for(int n = 0; n < N_CIRCLES; n++){
        QColor color(circles[n].red(),
                     circles[n].green(),
                     circles[n].blue(),
                     circles[n].alpha());
        painter.setBrush(QBrush(color));
        painter.drawEllipse(circles[n].center(), circles[n].radius(), circles[n].radius());
    }
    painter.end();
}

unsigned long long Distance(Chromosome *dna){
    int nBytesImage = sourceImage.byteCount();
    QImage imageRender = DrawImage(dna);
    unsigned long long distance = 0;
    const unsigned char *original = sourceImage.bits();
    const unsigned char *render = imageRender.bits();

    // get the distance
    for (int i = 0; i < nBytesImage; i++) {
        int temp = original[i] - render[i];
        distance += temp*temp;
    }

    return distance;
}

void Crossover(Chromosome *a, Chromosome *b){
    int length = a->Length();

    if(length <= 0)
        return;

    for(int i = 0; i < length; i++){
        // switch current circle. 50% prob.
        if(rand() % 2){
            Circle tmp = a->DNA()[i];
            a->DNA()[i] = b->DNA()[i];
            b->DNA()[i] = tmp;
        }
    }
}

Chromosome *SelectBest(){
    if(sizePopulationA == 0)
        return NULL;

    unsigned long long min = ~0x00;
    int index = 0;

    for(int i = 0; i < sizePopulationA; i++){
        if(populationA[i]->Fitness() < min){
            min = populationA[i]->Fitness();
            index = i;
        }
    }
    return populationA[index];
}

Chromosome *SelectTournament(){
    int i = rand() % sizePopulationA;
    int j = rand() % sizePopulationA;

    if(populationA[i]->Fitness() < populationA[j]->Fitness())
        return populationA[i];
    else
        return populationA[j];
}

void InsertPobB(Chromosome *dna){
    if(sizePopulationA <= sizePopulationB)
        return;

    if(populationB[sizePopulationB] == NULL)
        populationB[sizePopulationB] = new Chromosome();

    dna->Clone(populationB[sizePopulationB]);
    sizePopulationB++;
}

void UpdatePopulation(){
    for(int i = 0; i < sizePopulationB; i++){
        //std::cout << "up: " << i << std::endl;
        populationA[i]->Delete();
        delete populationA[i];
        populationA[i] = populationB[i];
        populationB[i] = NULL;
    }

    sizePopulationA = sizePopulationB;
    sizePopulationB = 0;
}

void GAStep(){
    Chromosome *elite = SelectBest();
    Chromosome clonA;
    Chromosome clonB;
    Chromosome *ap = NULL;

    // Elitism
    elite->Clone(&clonA);
    InsertPobB(&clonA);

    // harem
    int fraction = int(HAREM * sizePopulationA);
    for(int i = 0; i < fraction; i += 2){
        elite->Clone(&clonA);
        SelectTournament()->Clone(&clonB);
        Crossover(&clonA, &clonB);
        clonA.Mutate();
        clonB.Mutate();

        clonA.Fitness() = Distance(&clonA);
        clonB.Fitness() = Distance(&clonB);

        InsertPobB(&clonA);
        InsertPobB(&clonB);
    }

    // Fill
    while((sizePopulationA - sizePopulationB) > 0){
        ap = SelectTournament();
        ap->Clone(&clonA);
        ap = SelectTournament();
        ap->Clone(&clonB);

        Crossover(&clonA, &clonB);
        clonA.Mutate();
        clonB.Mutate();
        clonA.Fitness() = Distance(&clonA);

        clonB.Fitness() = Distance(&clonB);

        if(clonA.Fitness() < clonB.Fitness())
            InsertPobB(&clonA);
        else
            InsertPobB(&clonB);
    }
    UpdatePopulation();
}

void RunGA(){
    int generations = 0;
    while(max_generation >= 0 && generations <= max_generation){
        GAStep();
        if(generations % 100 == 0){
            std::cout << "Generacion: " << generations << "\n";
        }
        if(generations % 1000 == 0){
            Chromosome *best = SelectBest();
            QImage image = DrawImage(best);
            image.save(QString("out/out_") + QString::number(generations) + ".png");
            qDebug() << "Fitness: " << best->Fitness();
        }
        generations++;
    }
    DrawSVG(SelectBest(), "Final.svg");
}

void InitGA(){
    sizePopulationA = sizePopulationB = 0;

    for(int i = 0; i < N_POPULATION; i++){
        // population A
        populationA[i] = new Chromosome();
        populationA[i]->Create(N_CIRCLES);
        Circle *chromo = populationA[i]->DNA();
        for(int j = 0; j < N_CIRCLES; j++){
            chromo[j].init(imgW, imgH);
        }
        populationA[i]->Fitness() = Distance(populationA[i]);
        sizePopulationA++;

        // population B
        populationB[i] = NULL;
    }
}

void CleanUp(){
    for(int i = 0; i < sizePopulationA; i++){
        if(populationA[i] != NULL){
            populationA[i]->Delete();
            delete populationA[i];
        }
        if(populationB[i] != NULL){
            populationB[i]->Delete();
            delete populationB[i];
        }
    }
}

// Entry point
int main(int argc, char *argv[]){
    const std::string helpString = "\nUsage: render [options] <image file>\n\n"
                                   "Options:\n"
                                   "\t-h\tShow this message and exit\n"
                                   "\t-g\tMaximum number of generations\n";
    int option;
    srand(time(NULL));

    opterr = 0;

    while((option = getopt(argc, argv, "hg:")) != -1){
        switch (option) {
            // get the generation limit
            case 'h':
                std::cout << helpString << std::endl;
                exit(0);
                break;
            case 'g': {
                max_generation = atoi(optarg);
                if(max_generation == 0){
                    std::cerr << "ERROR: Especificar limite de generarion mayor a cero.\nExample: -g 100\n";
                    exit(EXIT_FAILURE);
                }
                else if(max_generation < 0){
                    std::cerr << "ERROR: El numero de generaciones debe ser positivo\n";
                    exit(EXIT_FAILURE);
                }
                break;
            }
            case '?':
                std::cout << helpString << std::endl;
                if(optopt == 'g')
                    std::cout << "Error: option -" << char(optopt) << " requires and argument." << std::endl;
                else if(isprint(optopt))
                    std::cerr << "Error: unknown option: -" << char(optopt) << std::endl;
                exit(EXIT_FAILURE);
            default:
                std::cout << helpString << std::endl;
                exit(0);
                break;;
        }
    }

    // get source file
    if(optind >= argc){
        std::cerr << "Especificar imagen a replicar\n";
        exit(EXIT_FAILURE);
    }

    // check if the file exists.
    FILE *fp = fopen(argv[optind], "r");
    if(fp == NULL){
        std::cerr << "ERROR: Imagen no existe: " << argv[optind] << "\n";
        exit(EXIT_FAILURE);
    }
    fclose(fp);

    // cargar imagen y hacer magia.
    sourceImage = QImage(argv[optind]);
    imgW = sourceImage.width();
    imgH = sourceImage.height();

    std::cout << "Maximo de generaciones: " << max_generation << "\n";
    QDir::current().mkpath("out");
    InitGA();
    RunGA();
    CleanUp();

    return 0;
}
