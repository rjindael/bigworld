// WGSL auto-aligns vector to its required 16-byte boundary and pads the
// struct's tail to match (12-byte vec3 -> 16), giving a 32-byte struct
// without any explicit padding fields (that would add real fields instead
// of padding, which is what previously doubled this to 48 bytes and broke
// pipeline creation against the 32-byte uniform buffer built in shader.c).
struct Uniforms {
    scalar: f32,
    vector: vec3<f32>,
};

@group(0) @binding(0) var<uniform> uniforms: Uniforms;

@vertex
fn vs_main(@location(0) position: vec3<f32>) -> @builtin(position) vec4<f32> {
    let angle = radians(uniforms.scalar);
    let cos_a = cos(angle);
    let sin_a = sin(angle);

    // Apply rotation about the Y axis
    var rotated: vec3<f32>;
    rotated.x = position.x * cos_a + position.z * sin_a;
    rotated.y = position.y;
    rotated.z = -position.x * sin_a + position.z * cos_a;

    return vec4<f32>(rotated, 1.0);
}
