# Graphics Renderer Optimizations

## Current Implementation Analysis

### ✅ What's Good:
- Batched vertex rendering (collects all vertices per frame)
- Single OpenGL draw call per frame flush
- Modern OpenGL 3.3 Core Profile
- Proper blending setup

### 🔧 Critical Optimizations Needed (Before Text Rendering):

#### 1. **Persistent Mapped Buffers (HIGH IMPACT)**
**Problem**: `glBufferSubData` causes GPU stalls by uploading data synchronously
**Solution**: Use `glMapBufferRange` with `GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT`
- Map once, write directly to GPU memory
- Unmap only on flush
- Can use double/triple buffering for async writes

#### 2. **Vertex Array Objects (VAO) (MEDIUM IMPACT)**
**Problem**: Setting vertex attrib pointers every flush is expensive
**Solution**: Create a VAO once at init, bind it on flush
- Single VAO with all vertex format setup
- Reduces state changes significantly

#### 3. **Projection Matrix Caching (MEDIUM IMPACT)**
**Problem**: Converting screen coords to NDC every vertex (per-frame divisions)
**Solution**: Pre-compute projection matrix once per resize
- Use uniform matrix in shader: `glUniformMatrix4fv`
- Transform in shader instead of CPU (faster, no per-vertex division)

#### 4. **Dynamic Vertex Buffer Growth (LOW-MEDIUM IMPACT)**
**Problem**: Fixed 10K vertex capacity - might overflow with many shapes
**Solution**: Grow buffer dynamically when needed
- Start smaller (4K), grow by 1.5x when full
- Use `glBufferData` with `NULL` to orphan old buffer (fast resize)

#### 5. **Remove Redundant State Changes (MEDIUM IMPACT)**
**Problem**: `glUseProgram`, `glBindBuffer`, `glEnableVertexAttribArray` called every flush
**Solution**: Track current state, only change when needed
- Check if program already bound before binding
- Cache current VBO binding

#### 6. **Optimize Circle Rendering (LOW IMPACT)**
**Problem**: 32 segments per filled circle = 96 vertices (3 verts × 32)
**Solution**: 
- Adaptive segments based on radius (smaller circles need fewer)
- Or use geometry shader to generate circle from center + radius

#### 7. **Line Rendering Optimization (LOW-MEDIUM IMPACT)**
**Problem**: Lines rendered as quads (2 triangles = 6 vertices)
**Solution**: Use `GL_LINES` primitive type for actual lines
- Separate vertex format for lines vs triangles
- Or use geometry shader for line-to-quad conversion

#### 8. **Color Format Optimization (LOW IMPACT)**
**Problem**: Converting uint8 colors to float every call (division)
**Solution**: Cache float color, update only when color changes
- Store float colors in renderer state
- Only convert when `set_color` called, not per-vertex

#### 9. **Batch by Primitive Type (MEDIUM IMPACT)**
**Problem**: Mixing lines and triangles requires separate draws
**Solution**: Separate batches for different primitive types
- Lines batch vs Triangles batch
- Flush each type separately with appropriate `glDrawArrays` mode

#### 10. **Disable VSync Optionally (LOW IMPACT)**
**Problem**: VSync limits FPS to monitor refresh rate (might want uncapped for testing)
**Solution**: Make VSync configurable
- Add renderer option for vsync on/off
- Useful for performance testing

### 📊 Performance Priority Ranking:

1. **PERSISTENT MAPPED BUFFERS** - Biggest CPU→GPU transfer bottleneck
2. **VAO Setup** - Reduces expensive state changes  
3. **PROJECTION MATRIX** - Eliminates per-vertex divisions
4. **State Caching** - Prevents redundant GL calls
5. **Dynamic Buffer Growth** - Prevents crashes/overflows
6. **Line Primitive Type** - Reduces vertex count for lines
7. **Batch by Primitive** - Better GPU efficiency
8. **Color Caching** - Minor CPU savings
9. **Adaptive Circle Segments** - Memory/vertex savings
10. **VSync Toggle** - Convenience feature

### 💡 Implementation Order Recommendation:

**Phase 1 (Critical for Performance):**
1. Persistent mapped buffers
2. VAO setup
3. Projection matrix

**Phase 2 (Important for Stability):**
4. State caching
5. Dynamic buffer growth

**Phase 3 (Nice to Have):**
6. Line primitive optimization
7. Batch separation
8. Other micro-optimizations

