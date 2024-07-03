# Heatmap

![heatmap](https://github.com/Tamvey/heatmap/assets/112703217/3a720ae1-c20d-4a1c-a0ec-5a4732bb4d45)

## Build 
```
cmake -S heatmap -B build -DOpenCV_DIR=<path_to_opencv_cmake_config>
cd build
cmake --build .
```
## Run
```
<heatmap_executable> <arg[1]> <arg[2]>
```
`<arg[1]>` - path to video file

`<arg[2]>` - path to property file

> [!NOTE]
> If no property file specified used default properties.
> 
> Loaded areas coords are written to `regions.txt` file.

## Control
Application controlled by keyboard events:

`h` - display current heatmap

`m` - display median frame

`esc` - exit

## Property file
`frames_for_median` - amount of frames used to find median frame

`min_contour_area` - minimal area of moving object to be detected

`usr_cols` - amount of user's grid columns

`usr_rows` - amount of user's grid rows

`gradient_colors` - amount of colors from blue to red

`color_threshold` - number of the coldest color detecting high workload

`min_area` - percentage of grid cell covered with warm color to be defined as high loaded

> [!NOTE]
> Example of property file structure [props.txt](https://github.com/Tamvey/heatmap/blob/main/props.txt).

