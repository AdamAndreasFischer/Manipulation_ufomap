To build the ufo library

From Root directory i.e PATH/ufo

```sudo apt install build-essential cmake git libpcl-dev libtbb-dev```

mkdir -p build \
cd build \
cmake .. \
make -j $(nproc) 

To run ply_to_pcd.bash install 
sudo apt install pcl-tools

#To run manipulation ufo map run from root dir:

./build/lib/map/apps/manipulation/UFOManipulation ./lib/map/apps/manipulation/config.toml

#In config the path to the pointcloud dir is specified. If pointclouds are not in pcd format, i.e saved with for exampl open3d, run

./lib/map/apps/manipulation/ply_to_pcd.bash /path/to/pointclouds # They can be in separate folders, the script will find all pcds

#To process poses, make sure they are saved as npz files in the same folder as the pointclouds then run

python3 lib/map/apps/manipulation/numpy_to_tsv.py /path/to/pointclouds_and_poses n_pcd_per_pose: int convert_to_meters: [true/false] switch_quat_order: [true/false]

The switch quat order changes quats from scalar last to scalar first

