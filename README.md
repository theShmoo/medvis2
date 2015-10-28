# Visualisierung Medizinischer Daten

 * Gruppe 6
 * Andreas Gogel - 0801243
 * David Pfahler - 1126287

## Thema

Wir wollen einen Direct Volume Renderer schreiben dessen 3D Daten zuerst von unterschiedlichen Filtern vorgefiltert wird. Ziel soll eine sinnvolle Darstellung von Blutgefäßen sein. Welche Filter genau verwendet werden soll der User (z.B.: durch Command Line Parameter oder eine kleine GUI) wählen. Das Filtering soll auf der GPU laufen und eventuell werden noch andere Interaktionsmöglichkeiten verwendet. (z.B.: 2D Transfer Functions) Außerdem wollen wir eine interaktive Kamera Steuerung einbauen.

## Techniken

### Kategorie 1

 * Windowing Function (5)
 * Gaussian Filtering (5)

### Kategorie 2

 * Bilateral Filtering (10)
 * 1D Transfer Function (10)

### Kategorie 3

 * Direct Volume Rendering (10)
 * vielleicht: 2D Transfer Function (10)

## Frameworks

Wir wollen das Visualization Tool Kit (VTK) verwenden gemeinsam mit C++

 * VTK – http://www.vtk.org/ 
