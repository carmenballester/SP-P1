// Carmen Ballester Bernabeu - 54204928T
// Sheila Sánchez Rodríguez - 49250479G

#include "cameraCalibration.h"

/**
 * @brief getParams Carga los parámetros intrínsecos de la cámara y los coeficientes de distorión desde un archivo .yaml
 * @param cameraMatrix Matriz que contiene los parámetros intrinsecos de la cámara
 * @param distCoeffs Matriz que contiene los coeficientes de distorsión de la cámara
 */
void getParams(Mat *cameraMatrix, Mat *distCoeffs) {

    // Abrir el fichero .yaml y guardas los parámetros en las variables
    FileStorage fs("calibration.yml", FileStorage::READ);
    fs["cameraMatrix"] >> *cameraMatrix;
    fs["distCoeffs"] >> *distCoeffs;
}

/**
 * @brief corners Detecta las esquinas del patrón de calibración para el frame actual
 * @param frame Imagen actual captada por la cámara
 * @return Vector de puntos 2D que contiene las coordenadas de las esquinas del tablero de calibración detectado en el frame
 */
vector<Point2f> corners(Mat frame) {
    vector<Point2f> corners;
    Mat imageGrey;

    // Comprobar si se detecta el patrón de calibración
    bool patternfound = findChessboardCorners(frame, patternSize, corners,
    CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);

    // Si se detecta, afinar la detección de las coordenadas de los píxeles de las esquinas
    if(patternfound) {
        cvtColor(frame, imageGrey, CV_BGR2GRAY);
        cornerSubPix(imageGrey, corners, Size(10, 10), Size(-1, -1),
                         TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
    }

    return corners;
}

/**
 * @brief calculatePoints Establece las coordenadas 3D de las esquinas del tablero ed calibración
 * @return Vector de puntos 3D que contiene las coordenadas 3D de las esquinas del tablero de calibración detectado en el frame
 */
vector<Point3f> calculatePoints() {
    vector<Point3f> points;

    for(int i=0; i<patternSize.height; i++) {
        for(int j=0; j<patternSize.width; j++) {
            points.push_back(Point3f(float( j*squareSize ), float( i*squareSize ), 0));
        }
    }

    return points;
}

/**
 * @brief calculateModelPoints Establece las coordenadas 3D de los puntos del modelo 3D que queremos proyectar en el tablero
 * @return Vector de puntos 3D que contiene las coordenadas 3D de los puntos del modelo 3D que queremos proyectar en el tablero
 */
vector<Point3f> calculateModelPoints() {
    vector<Point3f> points;

    points.push_back(Point3f(float(2*squareSize),float(2*squareSize),0));
    points.push_back(Point3f(float(2*squareSize),float(4*squareSize),0));
    points.push_back(Point3f(float(2*squareSize),float(3*squareSize),-float(2*squareSize)));
    points.push_back(Point3f(float(2*squareSize),float(3*squareSize),-float(4*squareSize)));
    points.push_back(Point3f(float(2*squareSize),float(2*squareSize),-float(3*squareSize)));
    points.push_back(Point3f(float(2*squareSize),float(4*squareSize),-float(3*squareSize)));

    return points;
}

int main () {
    vector<Point3f> realPoints, modelRealPoints;
    vector<Point2f> imagePoints, modelImagePoints;

    Mat cameraMatrix;
    Mat distCoeffs;
    Mat rvecs, tvecs;

    VideoCapture capture;
    Mat frame;
    Mat result;
    
    getParams(&cameraMatrix, &distCoeffs); // Obtener los parámetros de la cámara

    realPoints = calculatePoints(); // Definir los puntos 3D del tablero de calibración
    modelRealPoints = calculateModelPoints(); // Definir los puntos 3D del modelo que vamos a proyectar

    capture = cv::VideoCapture(0);
    while(1) {
        capture.read(frame);
        result = frame;

        imagePoints = corners(frame); // Detectar las esquinas del patrón en el frame de la cámara capturado

        // Comprobar que se detectan todas las esquinas del tablero de calibración
        if((imagePoints.size() != 0) && (imagePoints.size() == realPoints.size())) {
            // Obtener los parámetros extrínsecos de la cámara
            solvePnP(realPoints, imagePoints, cameraMatrix, distCoeffs, rvecs, tvecs);
            // Utilizar las matrices de traslación y rotación para obtener la transformación de los puntos 3D del modelo a puntos 2D de la imagen y proyectarlos correctamente
            projectPoints(modelRealPoints, rvecs, tvecs, cameraMatrix, distCoeffs, modelImagePoints);

            // Dibujar las líneas en la imagen uniendo los puntos 2D del modelo obtenidos
            line(result,modelImagePoints[0],modelImagePoints[2],Scalar(0,0,255),2,8);
            line(result,modelImagePoints[1],modelImagePoints[2],Scalar(0,0,255),2,8);
            line(result,modelImagePoints[2],modelImagePoints[3],Scalar(0,0,255),2,8);
            line(result,modelImagePoints[4],modelImagePoints[5],Scalar(0,0,255),2,8);
            circle(result,modelImagePoints[3],10,Scalar(0,0,255),2,8);
        }

        imshow("result", result);
        waitKey(50);
    }

    return 0;
}
