// Carmen Ballester Bernabeu - 54204928T
// Sheila Sánchez Rodríguez - 49250479G

#include "cameraCalibration.h"

/**
 * @brief loadImages Carga las imagenes del dataset de calibración
 * @return Vector de matrices que contiene las imágenes cargadas
 */
vector<Mat> loadImages() {
    Mat image;
    vector<Mat> images;

    // Crear un vector con los nombres de todas las imagenes del dataset de calibración
    String folder = "imagesCircle/*.jpg";
    vector<String> filenames;
    glob(folder, filenames);

    // Leer cada una de las imágenes del dataset de calibración y almacenarla en la matriz de imágenes si se pueden leer correctamente
    for(size_t i=0; i<filenames.size(); i++) {
        image = imread(filenames[i]);
        imageSize = image.size();
        if (!image.data ) {
            cout <<  "Could not open or find the image" << endl;
            exit(-1);
        }
        images.push_back(image);
    }

    return images;
}

/**
 * @brief corners Detecta las esquinas del patrón de calibración para cada una de las imágenes del dataset de calibración
 * @param images Vector de matrices que contiene todas las imágenes cargadas del dataset de calibración
 * @return Matriz de puntos 2D que contiene las coordenadas de las esquinas detectadas en cada imagen para cada imagen del dataset de calibración
 */
vector<vector<Point2f>> corners(vector<Mat> images) {
    vector<vector<Point2f>> corners(images.size());
    Mat imageGrey;

    // Para cada imagen del dataset comprobar si se dececta el patrón de calibración
    for(size_t i=0; i<images.size(); i++) {
        bool patternfound = findCirclesGrid(images[i], patternSizeC, corners[i]);

        // Si se detecta, afinar la detección de las coordenadas de los píxeles de las esquinas
        if(patternfound) {
            // Dibujar el patrón detectado en la imagen
            drawChessboardCorners(images[i], patternSizeC, corners[i], patternfound);
            imshow("Pattern found", images[i]);
            waitKey(0);
        }

        else {
            cout <<  "Could not find the calibration pattern" << endl;
            exit(-1);
        }
    }
    return corners;
}

/**
 * @brief calculatePoints Establece las coordenadas 3D de las esquinas del tablero ed calibración
 * @param images Vector de matrices que contiene todas las imágenes cargadas del dataset de calibración
 * @return Matriz de puntos 3D que contiene las coordenadas 3D de las esquinas del tablero de calibración
 */
vector<vector<Point3f>> calculatePoints(vector<Mat> images) {
    vector<vector<Point3f>> pointsMat;
    vector<Point3f> points;

    // Crear un vector de puntos 3D con las coordenadas del mundo real del tablero de calibración
    for(int i=0; i<patternSizeC.height; i++) {
        for(int j=0; j<patternSizeC.width; j++) {
            points.push_back(Point3f(float( j*circleSize ), float( i*circleSize ), 0));
        }
    }

    // Crear una matriz donde cada fila contiene este vector, con el mismo número de filas que imágenes en el dataset de calibración
    for(size_t i=0; i<images.size(); i++) {
        pointsMat.push_back(points);
    }

    return pointsMat;
}

/**
 * @brief saveCalibration
 * @param cameraMatrix Matriz que contiene los parámetros intrinsecos de la cámara
 * @param distCoeffs Matriz que contiene los coeficientes de distorsión de la cámara
 */
void saveCalibration (Mat cameraMatrix, Mat distCoeffs) {
    FileStorage fs("calibration.yml", FileStorage::WRITE);
    time_t rawtime;

    // Guardar la fecha de calibración, la matriz de parámetros intrínsecos de la cámara y los coeficientes de distorsión en un fichero .yaml
    time(&rawtime);
    fs << "calibrationDate" << asctime(localtime(&rawtime));
    fs << "cameraMatrix" << cameraMatrix << "distCoeffs" << distCoeffs;

    fs.release();
}

int main () {
    vector<Mat> images;
    vector<vector<Point3f> >realPoints;
    vector<vector<Point2f> > imagePoints;

    Mat cameraMatrix;
    Mat distCoeffs;
    vector<Mat> rvecs, tvecs;

    images = loadImages(); // Cargar imágenes
    imagePoints = corners(images); // Detectar las esquinas en 2D para cada imagen
    realPoints = calculatePoints(images); // Generar las esquinas 3D para cada imagen

    // Comprobar que se han detectado bien los puntos en la imagen del dataset de calibración y que se puede calibrar la cámara
    if (imagePoints.size() == realPoints.size()) {
        double rms = calibrateCamera(realPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs, CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
        cout << "error: " << rms << endl;

        saveCalibration(cameraMatrix, distCoeffs);
    }

    return 0;
}
