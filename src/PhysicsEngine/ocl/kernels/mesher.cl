// Mesher system, it uses TSDF voxel grid construction then marching cube to
// reconstruct fluid suface

// TSDF_GRID_RES            - TSDF grid resolution
// TSDF_GRID_CELL_SIZE      - TSDF number of cells
// TSDF_GRID_NUM_CELLS      - TSDF grid number of cells
// ABS_WALL_POS             - Absolute position of the walls in x,y,z
// TSDF_NUM_MAX_PARTS_IN_CELL   - maximum number of particles taking into
// account in a single cell in simplified mode

#define ID get_global_id(0)

/*
  Compute 3D index of the cell containing given position
*/
inline uint3 TSDF_getCell3DIndexFromPos(float4 pos) {
  // Moving particles in [0 - 2 * ABS_WALL_POS] to have coords matching with
  // cellIndices
  const float3 posXYZ =
      clamp(pos.xyz, -ABS_WALL_POS, ABS_WALL_POS) + (float3)(ABS_WALL_POS);

  const uint3 cell3DIndex = convert_uint3(floor(posXYZ / TSDF_GRID_CELL_SIZE));

  return cell3DIndex;
}

/*
  Compute 1D index of the cell containing given position
*/
inline uint TSDF_getCell1DIndexFromPos(float4 pos) {
  const uint3 cell3DIndex = TSDF_getCell3DIndexFromPos(pos);

  const uint cell1DIndex = cell3DIndex.x * TSDF_GRID_RES * TSDF_GRID_RES +
                           cell3DIndex.y * TSDF_GRID_RES + cell3DIndex.z;

  return cell1DIndex;
}

/*
  Reset cellID buffer. For radix sort purpose.
*/
__kernel void TSDF_resetCellIDs(__global uint *TSDFCellID) {
  // For all particles, giving cell ID above any available one
  // the ones not filled later (i.e not processed because index > nbParticles
  // displayed) will be sorted at the end and not considered after sorting
  TSDFCellID[ID] = TSDF_GRID_NUM_CELLS * 2 + ID;
}

/*
  Fill cellID buffer. For radix sort purpose.
*/
__kernel void TSDF_fillCellIDs( // Input
    const __global float4 *TSDFPartPosTmp,
    // Output
    __global uint *TSDFCellID) {
  const float4 pos = TSDFPartPosTmp[ID];

  const uint cell1DIndex = TSDF_getCell1DIndexFromPos(pos);

  TSDFCellID[ID] = cell1DIndex;
}

/*
  Reset startEndPartID buffer for each cell.
*/
__kernel void TSDF_resetStartEndCell(__global uint2 *TSDFPartStartEndID) {
  // Resetting with 1 as starting index and 0 as ending index
  // Little hack to bypass empty cell further
  TSDFPartStartEndID[ID] = (uint2)(1, 0);
}

/*
  Find first partID for each cell.
*/
__kernel void TSDF_fillStartCell( // Input
    const __global uint *TSDFCellID,
    // Output
    __global uint2 *TSDFPartStartEndID) {
  const uint currentCellID = TSDFCellID[ID];

  if (ID > 0 && currentCellID < TSDF_GRID_NUM_CELLS) {
    uint leftCellID = TSDFCellID[ID - 1];
    if (currentCellID != leftCellID) {
      // Found start
      TSDFPartStartEndID[currentCellID].x = ID;
    }
  }
}

/*
  Find last partID for each cell.
*/
__kernel void TSDF_fillEndCell( // Input
    const __global uint *TSDFCellID,
    // Output
    __global uint2 *TSDFPartStartEndID) {
  const uint currentCellID = TSDFCellID[ID];

  if (ID != get_global_size(0) && currentCellID < TSDF_GRID_NUM_CELLS) {
    const uint rightCellID = TSDFCellID[ID + 1];
    if (currentCellID != rightCellID) {
      // Found end
      TSDFPartStartEndID[currentCellID].y = ID;
    }
  }
}

/*
  Adjust last partID for each cell, capping it with max number of parts in cell
  in simplified mode.
*/
__kernel void TSDF_adjustEndCell(__global uint2 *TSDFPartStartEndID) {
  const uint2 startEnd = TSDFPartStartEndID[ID];

  if (startEnd.y > startEnd.x) {
    const uint newEnd = startEnd.x + min(startEnd.y - startEnd.x,
                                         (uint)TSDF_NUM_MAX_PARTS_IN_CELL);
    TSDFPartStartEndID[ID] = (uint2)(startEnd.x, newEnd);
  }
}

/*
    Compute TSDF grid value, using iso kernel
*/
__kernel void TSDF_computeGrid(
    // Inputs
    const __global uint *TSDFCellID, const __global uint2 *TSDFPartStartEndID,
    const __global float4 *TSDFPartPosTmp,
    // output
    __global float4 *TSDFGrid) {}