#include <d3dcompiler.h>
#include <directxmath.h>
#include <iostream>

#include "Camera.h"

using namespace DirectX::SimpleMath;

void Camera::Initialize(InputDevice* inputDevice) {
    inpDevice = inputDevice;

    D3D11_BUFFER_DESC constBufDesc = {};
    constBufDesc.Usage = D3D11_USAGE_DYNAMIC;
    constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constBufDesc.MiscFlags = 0;
    constBufDesc.StructureByteStride = 0;
    constBufDesc.ByteWidth = sizeof(viewProjection);

    device->CreateBuffer(&constBufDesc, nullptr, &cb);

    proj = Matrix::CreatePerspectiveFieldOfView(
        DirectX::XMConvertToRadians(45), 1, 1, 1000);
    look = Matrix::CreateLookAt({ 10,10,10 }, { 0,0,0 }, { 0,1,0 });
    position = Vector3(radiusCamera, radiusCamera, radiusCamera);
    Vector3 direction = Vector3(0.0f, 0.0f, 0.0f) - position;
    direction.Normalize();
    yaw = std::atan2(direction.x, direction.z);
    pitch = std::asin(direction.y);

}

void Camera::Draw() {
    UINT strides[] = { 32 };
    UINT offsets[] = { 0 };
    context->VSSetConstantBuffers(1, 1, &cb);
    context->IASetInputLayout(layout);
}

void Camera::Update() {
    D3D11_MAPPED_SUBRESOURCE res = {};

    if (inpDevice->IsKeyDown(Keys::Tab) && !tabPressedLastFrame) {
        if (mode == CameraMode::Orbit) {
            mode = CameraMode::FPS;

            float x = radiusCamera * sinf(0.0001) * cosf(teta);
            float y = radiusCamera * cosf(0.0001);
            float z = radiusCamera * sinf(0.0001) * sinf(0);
            // Направление от камеры к центру (или к lookAt)
            position = Vector3(x, y, z);
            Vector3 dir = lookAt - position;
            dir.Normalize();

            yaw = std::atan2(dir.x, dir.z);
            pitch = std::asin(dir.y);
        }
        else if (mode == CameraMode::FPS)
            mode = CameraMode::Orbit;
    }
    tabPressedLastFrame = inpDevice->IsKeyDown(Keys::Tab);

    if (mode == CameraMode::Orbit) {

        if /*(inpDevice->IsKeyDown(Keys::W)) {
            deltax = 1;
        }
        else if*/ (inpDevice->IsKeyDown(Keys::D)) {
            deltay = 1;
        }
        else if (inpDevice->IsKeyDown(Keys::A)) {
            deltay = -1;
        }
       /* else if (inpDevice->IsKeyDown(Keys::S)) {
            deltax = -1;
        }*/
        else {
            deltax = 0;
            deltay = 0;
        }

        phi = DirectX::XMConvertToRadians(DirectX::XMConvertToDegrees(phi) - deltax);
        teta = DirectX::XMConvertToRadians(DirectX::XMConvertToDegrees(teta) - deltay);

        if (phi <= 0.0001) phi = 0.0001;
        if (phi >= 1.45) phi = 1.45;

        Vector3 up = Vector3::Up;
        double wrapped = fmod(DirectX::XMConvertToDegrees(phi) + 180.0, 360.0);
        if (wrapped < 0) wrapped += 360.0;
        wrapped -= 180.0;
        if (wrapped > 0 && wrapped < 180) {
            up = Vector3::Up;
        }
        else if (wrapped < 0 && wrapped > -180) {
            up = Vector3::Down;
        }
        else if (wrapped == 0) {
            up = Vector3::Up;
        }
        else { // wrapped == ±180
            up = Vector3::Down;
        }

        float moveSpeed = 0.1f;
        if (inpDevice->IsKeyDown(Keys::W) /* || inpDevice->IsKeyDown(Keys::Down)*/) {
            // Направление от камеры к цели
            Vector3 viewDir = (targetCenter - position);
            viewDir.y = 0.0f; // Проекция на XZ
            if (viewDir.LengthSquared() > 0.001f) {
                viewDir.Normalize();
                // Определяем направление движения
                if (inpDevice->IsKeyDown(Keys::W)) {
                    targetCenter += viewDir * moveSpeed;
                }
                //else {
                //    targetCenter -= viewDir * moveSpeed;
                //}
            }
        }

        // Сферические координаты камеры
        float x = radiusCamera * sinf(phi) * cosf(teta);
        float y = radiusCamera * cosf(phi);
        float z = radiusCamera * sinf(phi) * sinf(teta);
        position = targetCenter + Vector3(x, y, z);
        lookAt = targetCenter;

        // Матрица камеры
        view = Matrix::CreateLookAt(position, lookAt, up);
    }
    else if (mode == CameraMode::FPS) {
        // FPS-логика
        Vector2 mouseOffset = inpDevice->MouseOffset;
        yaw -= mouseOffset.x * 0.008f;
        pitch -= mouseOffset.y * 0.008f;

        // Ограничение pitch
        float limit = DirectX::XM_PIDIV2 - 0.01f;
        if (pitch > limit) {
            pitch = limit;
        }
        else if (pitch < -limit) {
            pitch = -limit;
        }

        Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(yaw, pitch, 0.0f);
        Vector3 forward = rotationMatrix.Forward();
        Vector3 right = Vector3::Transform(Vector3::Right, rotationMatrix);

        float moveSpeed = 0.1f; // Скорость движения (можно регулировать)
        if (inpDevice->IsKeyDown(Keys::W)) {
            position += forward * moveSpeed;
        }
        if (inpDevice->IsKeyDown(Keys::S)) {
            position -= forward * moveSpeed;
        }
        if (inpDevice->IsKeyDown(Keys::A)) {
            position -= right * moveSpeed;
        }
        if (inpDevice->IsKeyDown(Keys::D)) {
            position += right * moveSpeed;
        }

        Vector3 look = position + forward;
        view = Matrix::CreateLookAt(position, look, Vector3::Up);
        inpDevice->MouseOffset = Vector2::Zero;
    }

    Matrix viewProj = view * proj;
    viewProjection = viewProj.Transpose();

    context->Map(cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

    auto dataPtr = reinterpret_cast<Matrix*>(res.pData);
    memcpy(dataPtr, &viewProjection, sizeof(viewProjection));
    context->Unmap(cb, 0);
}

void Camera::DestroyResources() {}

void Camera::GoToStandart() {
    if (mode == CameraMode::Orbit) {
        deltax = 0;
        deltay = 0;
        phi = 0.0001f;
        teta = 0;
        targetCenter = Vector3::Zero;
    }
}


