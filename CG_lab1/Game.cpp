#include "Game.h"

void Game::Initialize() {

    display = new DisplayWin32(L"My3DApp", 800, 800);

    D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    swapDesc.BufferCount = 2;
    swapDesc.BufferDesc.Width = 800;
    swapDesc.BufferDesc.Height = 800;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = display->hWnd;
    swapDesc.Windowed = true;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.SampleDesc.Quality = 0;

    auto res = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_DEBUG,
        featureLevel,
        1,
        D3D11_SDK_VERSION,
        &swapDesc,
        &swapChain,
        &device,
        nullptr,
        &context);

    ID3D11Texture2D* backTex;
    res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backTex);	// __uuidof(ID3D11Texture2D)
    res = device->CreateRenderTargetView(backTex, nullptr, &rtv);

    inpDevice = new InputDevice(display);
    display->setInputDevice(inpDevice);
    ID3D11Texture2D* depthStencilTexture = nullptr;

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = 800;
    depthDesc.Height = 800;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    device->CreateTexture2D(&depthDesc, nullptr, &depthStencilTexture);

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = depthDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    device->CreateDepthStencilView(depthStencilTexture, &dsvDesc, &dsv);


#pragma region Pong
    //std::vector<int> indeces = { 0,1,2, 1,0,3 };

    //std::vector<DirectX::XMFLOAT4> points = {
    //DirectX::XMFLOAT4(-0.8f, 0.5f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //DirectX::XMFLOAT4(-0.9f, -0.5f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //DirectX::XMFLOAT4(-0.8f, -0.5f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //DirectX::XMFLOAT4(-0.9f, 0.5f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
    //};

    //components.push_back(new RectangleComponent(device.Get(), context, points, indeces, {0.0f, 0.0f}));

    //std::vector<DirectX::XMFLOAT4> points2 = {
    //DirectX::XMFLOAT4(0.8f, 0.5f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //DirectX::XMFLOAT4(0.9f, -0.5f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //DirectX::XMFLOAT4(0.8f, -0.5f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //DirectX::XMFLOAT4(0.9f, 0.5f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
    //};

    //components.push_back(new RectangleComponent(device.Get(), context, points2, indeces, { 0.0f, 0.0f }));

    //std::vector<DirectX::XMFLOAT4> pointsForBall = GenerateCirclePoints(0.0f, 0.0f, 0.05f);
    //std::vector<int> indecesBall = GenerateCircleIndeces();

    //components.push_back(new RectangleComponent(device.Get(), context, pointsForBall, indecesBall, GenerateRandomBallDirection()));


    //std::vector<DirectX::XMFLOAT4> plankUp = {
    //DirectX::XMFLOAT4(1.0f, -1.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //DirectX::XMFLOAT4(-1.0f, -1.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //DirectX::XMFLOAT4(1.0f, -0.9f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //DirectX::XMFLOAT4(-1.0f, -0.9f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
    //};
    //components.push_back(new RectangleComponent(device.Get(), context, plankUp, indeces, { 0.0f, 0.0f }));

    //std::vector<DirectX::XMFLOAT4> plankDown = {
    //DirectX::XMFLOAT4(1.0f, 1.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //DirectX::XMFLOAT4(-1.0f, 1.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //DirectX::XMFLOAT4(1.0f, 0.9f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //DirectX::XMFLOAT4(-1.0f, 0.9f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
    //};
    //components.push_back(new RectangleComponent(device.Get(), context, plankDown, indeces, { 0.0f, 0.0f }));

    //std::vector<DirectX::XMFLOAT4> plankLeft = {
    //DirectX::XMFLOAT4(-1.0f, 1.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //DirectX::XMFLOAT4(-1.0f, -1.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //DirectX::XMFLOAT4(-0.9f, 1.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //DirectX::XMFLOAT4(-0.9f, -1.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
    //};
    //components.push_back(new RectangleComponent(device.Get(), context, plankLeft, indeces, { 0.0f, 0.0f }));

    //std::vector<DirectX::XMFLOAT4> plankRight = {
    //DirectX::XMFLOAT4(1.0f, 1.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //DirectX::XMFLOAT4(1.0f, -1.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //DirectX::XMFLOAT4(0.9f, 1.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
    //DirectX::XMFLOAT4(0.9f, -1.0f, 0.5f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
    //};
    //components.push_back(new RectangleComponent(device.Get(), context, plankRight, indeces, { 0.0f, 0.0f }));


    //components[0]->Initialize();
    //components[1]->Initialize();
    //components[2]->Initialize();
    //components[3]->Initialize();
    //components[4]->Initialize();
    //components[5]->Initialize();
    //components[6]->Initialize();

#pragma endregion Pong 

#pragma region CubeMesh
    //std::vector<int> indices = {
    //    // Передняя грань
    //    0, 1, 2,  0, 2, 3,
    //    // Задняя грань
    //    4, 6, 5,  4, 7, 6,
    //    // Левая грань
    //    4, 5, 1,  4, 1, 0,
    //    // Правая грань
    //    3, 2, 6,  3, 6, 7,
    //    // Верхняя грань
    //    4, 0, 3,  4, 3, 7,
    //    // Нижняя грань
    //    1, 5, 6,  1, 6, 2
    //};

    //std::vector<DirectX::XMFLOAT4> points = {
    //    //          X      Y      Z      W      R    G    B    A
    //    { -0.5f,  0.5f, -0.5f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, // 0 - Верхний левый передний (красный)
    //    {  0.5f,  0.5f, -0.5f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, // 1 - Верхний правый передний (зеленый)
    //    {  0.5f, -0.5f, -0.5f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, // 2 - Нижний правый передний (синий)
    //    { -0.5f, -0.5f, -0.5f, 1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f }, // 3 - Нижний левый передний (желтый)

    //    { -0.5f,  0.5f,  0.5f, 1.0f }, { 1.0f, 0.0f, 1.0f, 1.0f }, // 4 - Верхний левый задний (розовый)
    //    {  0.5f,  0.5f,  0.5f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f }, // 5 - Верхний правый задний (голубой)
    //    {  0.5f, -0.5f,  0.5f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, // 6 - Нижний правый задний (белый)
    //    { -0.5f, -0.5f,  0.5f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }  // 7 - Нижний левый задний (черный)
    //};
    /*components.push_back(new NewComponent(device.Get(), context, points, indices, 2.0f, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1.0f));
    components[0]->Initialize();*/
#pragma endregion CubeMesh

#pragma region ManyRandomizedObjects
    //for (int i = 0; i < 1000; i++) {
    //    std::random_device rd;
    //    std::mt19937 gen(rd());

    //    // Для случайного выбора целых чисел
    //    std::uniform_int_distribution<int> intDist(0, 1);  // для bool и axis
    //    std::uniform_real_distribution<float> floatDist(0.0f, 1.0f);  // для значений типа float

    //    // 1. isSphere (будет 1 или 2)
    //    int isSphere = intDist(gen) + 1;  // результат будет либо 1, либо 2

    //    // 2. orbitRadius (от 0.0 до 20.0)
    //    std::uniform_real_distribution<float> orbitRadiusDist(0.0f, 5.0f);
    //    float orbitRadius = orbitRadiusDist(gen);

    //    // 3. rotationAxis (x, y, z могут быть 1 или 0, но не все нули)
    //    DirectX::SimpleMath::Vector3 rotationAxis;
    //    do {
    //        rotationAxis.x = intDist(gen);
    //        rotationAxis.y = intDist(gen);
    //        rotationAxis.z = intDist(gen);
    //    } while (rotationAxis.x == 0 && rotationAxis.y == 0 && rotationAxis.z == 0);  // избегаем всех нулевых значений

    //    // 4. centerCoords (x, y, z от 0.0 до 20.0)
    //    std::uniform_real_distribution<float> coordsDist(0.0f, 5.0f);
    //    DirectX::SimpleMath::Vector3 centerCoords(coordsDist(gen), coordsDist(gen), coordsDist(gen));

    //    // 5. spinVelocity (от 0.1 до 5.0)
    //    std::uniform_real_distribution<float> spinVelocityDist(0.1f, 5.0f);
    //    float spinVelocity = spinVelocityDist(gen);

    //    if (isSphere == 1)
    //        components.push_back(new NewComponent(device.Get(), context, points2, indices2, orbitRadius, rotationAxis, centerCoords, spinVelocity));
    //    else
    //        components.push_back(new NewComponent(device.Get(), context, points, indices, orbitRadius, rotationAxis, centerCoords, spinVelocity));
    //    components[i+5]->Initialize();
    //    std::uniform_real_distribution<float> scaleDist(1.0f, 5.0f);
    //    float scale = scaleDist(gen);
    //    components[i + 5]->scale /= scale;
    //}
#pragma endregion ManyRandomizedObjects

    camera = new Camera(device.Get(), context);
    terrain = new Terrain(device.Get(), context, 100.0f, 100.0f, 200);
    

    std::string path = "./chest/teasure_chest.obj";
    std::string pathMtl = "./chest/teasure_chest.mtl";

    std::string path2 = "./fox/source/LowPolyAnimal.obj";
    std::string path3 = "./candy-shop/source/Candy_shop.obj";


    //stickers.push_back(new StickyObject(device.Get(), context, { -10.0f, terrain->GetHeightAt(5.0f, 0.0f) + 0.3f, 0.0f }, inpDevice));
    stickers.push_back(new StickyObject(device.Get(), context, { -5.f, terrain->GetHeightAt(5.0f, 0.0f) + 0.3f, 0.0f }, inpDevice, path, pathMtl));
    //stickers.push_back(new StickyObject(device.Get(), context, { -7.5f, terrain->GetHeightAt(5.0f, 0.0f) + 0.3f, 0.0f }, inpDevice));


    ball = new NewComponent(device.Get(), context, inpDevice, { 1.0f, 0.0f, 0.0f }, { 0.0f,1.0f,0.0f }, 0.0f);
    ball->Initialize();

   /* grid = new Grid(device.Get(), context, 50.0f, 100);
    grid->Initialize();*/

    
    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = D3D11_CULL_FRONT;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    //rastDesc.FillMode = D3D11_FILL_WIREFRAME;

    res = device->CreateRasterizerState(&rastDesc, &rastState);

    context->RSSetState(rastState);
    camera->Initialize(inpDevice);
}

void Game::Run() {
    std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();
    float totalTime = 0;
    unsigned int frameCount = 0;
    MSG msg = {};
    while (!isExitRequested) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (msg.message == WM_QUIT) isExitRequested = true;

        context->ClearState();
        context->RSSetState(rastState);

        D3D11_VIEWPORT viewport = {};
        viewport.Width = static_cast<float>(width);
        viewport.Height = static_cast<float>(height);
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.MinDepth = 0;
        viewport.MaxDepth = 1.0f;


        D3D11_RECT scizors = {};
        scizors.left = 0;
        scizors.top = 0;
        scizors.bottom = MAXINT;
        scizors.right = MAXINT;
        context->RSSetViewports(1, &viewport);
        context->RSSetScissorRects(1, &scizors);
        context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context->OMSetRenderTargets(1, &rtv, dsv);

        /*auto	curTime = std::chrono::steady_clock::now();
        float	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
        PrevTime = curTime;

        totalTime += deltaTime;
        frameCount++;

        if (totalTime > 1.0f) {
            float fps = frameCount / totalTime;

            totalTime -= 1.0f;

            WCHAR text[256];
            swprintf_s(text, TEXT("FPS: %f"), fps);
            SetWindowText(display->hWnd, text);

            frameCount = 0;
        }*/

        //float color[] = { totalTime, 0.1f, 0.1f, 1.0f };
        float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        context->ClearRenderTargetView(rtv, color);
        context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


#pragma region Pong
        //components[0]->Draw();
        //components[1]->Draw();
        //components[2]->Draw();
        //components[3]->Draw();
        //components[4]->Draw();
        //components[5]->Draw();
        //components[6]->Draw();


        //context->OMSetRenderTargets(0, nullptr, nullptr);

        //components[2]->BallMovement();
        //components[2]->Update();

        //if (inpDevice->IsKeyDown(Keys::W)) {
        //    components[0]->offset.y += 0.01f;
        //    components[0]->Update();
        //}
        //if (inpDevice->IsKeyDown(Keys::S)) {
        //    components[0]->offset.y -= 0.01f;
        //    components[0]->Update();
        //}

        //if (inpDevice->IsKeyDown(Keys::Up)) {
        //    components[1]->offset.y += 0.01f;
        //    components[1]->Update();
        //}
        //if (inpDevice->IsKeyDown(Keys::Down)) {
        //    components[1]->offset.y -= 0.01f;
        //    components[1]->Update();
        //}


        //auto rectPtr = components[0];
        //auto rectPtr2 = components[2];

        //auto dxrect1 = rectPtr->rect.toRectangle();
        //auto dxrect2 = rectPtr2->rect.toRectangle();


        //auto intersectionRect = dxrect1.Intersect(dxrect1, dxrect2);
        //if (intersectionRect.width && intersectionRect.height) {
        //    // collided, calculate collision normal and call collision callback
        //    DirectX::SimpleMath::Vector2 collisionNormal;
        //    if (intersectionRect.height > intersectionRect.width) {
        //        collisionNormal = { -1, 0 };
        //        if (rectPtr->rect.x + rectPtr->rect.w / 2 > rectPtr2->rect.x + rectPtr2->rect.w / 2) {
        //            collisionNormal *= -1;
        //        }
        //    }
        //    else {
        //        collisionNormal = { 0, -1 };
        //        if (rectPtr->rect.y + rectPtr->rect.h / 2 > rectPtr2->rect.y + rectPtr2->rect.h / 2) {
        //            collisionNormal *= -1;
        //        }
        //    }
        //    collisionNormal *= -1;
        //    rectPtr2->ChangeBallDirection(collisionNormal);
        //    rectPtr->recalculateRect();
        //}

        //rectPtr = components[1];
        //rectPtr2 = components[2];

        //dxrect1 = rectPtr->rect.toRectangle();
        //dxrect2 = rectPtr2->rect.toRectangle();


        //intersectionRect = dxrect1.Intersect(dxrect1, dxrect2);
        //if (intersectionRect.width && intersectionRect.height) {
        //    // collided, calculate collision normal and call collision callback
        //    DirectX::SimpleMath::Vector2 collisionNormal;
        //    if (intersectionRect.height > intersectionRect.width) {
        //        collisionNormal = { -1, 0 };
        //        if (rectPtr->rect.x + rectPtr->rect.w / 2 > rectPtr2->rect.x + rectPtr2->rect.w / 2) {
        //            collisionNormal *= -1;
        //        }
        //    }
        //    else {
        //        collisionNormal = { 0, -1 };
        //        if (rectPtr->rect.y + rectPtr->rect.h / 2 > rectPtr2->rect.y + rectPtr2->rect.h / 2) {
        //            collisionNormal *= -1;
        //        }
        //    }
        //    collisionNormal *= -1;
        //    rectPtr2->ChangeBallDirection(collisionNormal);
        //    rectPtr->recalculateRect();
        //}

        //rectPtr = components[3];
        //rectPtr2 = components[2];

        //dxrect1 = rectPtr->rect.toRectangle();
        //dxrect2 = rectPtr2->rect.toRectangle();


        //intersectionRect = dxrect1.Intersect(dxrect1, dxrect2);
        //if (intersectionRect.width && intersectionRect.height) {
        //    // collided, calculate collision normal and call collision callback
        //    DirectX::SimpleMath::Vector2 collisionNormal;
        //    if (intersectionRect.height > intersectionRect.width) {
        //        collisionNormal = { -1, 0 };
        //        if (rectPtr->rect.x + rectPtr->rect.w / 2 > rectPtr2->rect.x + rectPtr2->rect.w / 2) {
        //            collisionNormal *= -1;
        //        }
        //    }
        //    else {
        //        collisionNormal = { 0, -1 };
        //        if (rectPtr->rect.y + rectPtr->rect.h / 2 > rectPtr2->rect.y + rectPtr2->rect.h / 2) {
        //            collisionNormal *= -1;
        //        }
        //    }
        //    collisionNormal *= -1;
        //    rectPtr2->ChangeBallDirection(collisionNormal);
        //}

        //rectPtr = components[4];
        //rectPtr2 = components[2];

        //dxrect1 = rectPtr->rect.toRectangle();
        //dxrect2 = rectPtr2->rect.toRectangle();


        //intersectionRect = dxrect1.Intersect(dxrect1, dxrect2);
        //if (intersectionRect.width && intersectionRect.height) {
        //    // collided, calculate collision normal and call collision callback
        //    DirectX::SimpleMath::Vector2 collisionNormal;
        //    if (intersectionRect.height > intersectionRect.width) {
        //        collisionNormal = { -1, 0 };
        //        if (rectPtr->rect.x + rectPtr->rect.w / 2 > rectPtr2->rect.x + rectPtr2->rect.w / 2) {
        //            collisionNormal *= -1;
        //        }
        //    }
        //    else {
        //        collisionNormal = { 0, -1 };
        //        if (rectPtr->rect.y + rectPtr->rect.h / 2 > rectPtr2->rect.y + rectPtr2->rect.h / 2) {
        //            collisionNormal *= -1;
        //        }
        //    }
        //    collisionNormal *= -1;
        //    rectPtr2->ChangeBallDirection(collisionNormal);
        //}

        //rectPtr = components[5];
        //rectPtr2 = components[2];

        //dxrect1 = rectPtr->rect.toRectangle();
        //dxrect2 = rectPtr2->rect.toRectangle();


        //intersectionRect = dxrect1.Intersect(dxrect1, dxrect2);
        //if (intersectionRect.width && intersectionRect.height) {
        //    // collided, calculate collision normal and call collision callback
        //    DirectX::SimpleMath::Vector2 collisionNormal;
        //    if (intersectionRect.height > intersectionRect.width) {
        //        collisionNormal = { -1, 0 };
        //        if (rectPtr->rect.x + rectPtr->rect.w / 2 > rectPtr2->rect.x + rectPtr2->rect.w / 2) {
        //            collisionNormal *= -1;
        //        }
        //    }
        //    else {
        //        collisionNormal = { 0, -1 };
        //        if (rectPtr->rect.y + rectPtr->rect.h / 2 > rectPtr2->rect.y + rectPtr2->rect.h / 2) {
        //            collisionNormal *= -1;
        //        }
        //    }
        //    collisionNormal *= -1;
        //    rectPtr2->ChangeBallDirection(collisionNormal);
        //    scoreRight += 1;
        //    printf("Score right: %d", scoreRight);
        //}

        //rectPtr = components[6];
        //rectPtr2 = components[2];

        //dxrect1 = rectPtr->rect.toRectangle();
        //dxrect2 = rectPtr2->rect.toRectangle();


        //intersectionRect = dxrect1.Intersect(dxrect1, dxrect2);
        //if (intersectionRect.width && intersectionRect.height) {
        //    // collided, calculate collision normal and call collision callback
        //    DirectX::SimpleMath::Vector2 collisionNormal;
        //    if (intersectionRect.height > intersectionRect.width) {
        //        collisionNormal = { -1, 0 };
        //        if (rectPtr->rect.x + rectPtr->rect.w / 2 > rectPtr2->rect.x + rectPtr2->rect.w / 2) {
        //            collisionNormal *= -1;
        //        }
        //    }
        //    else {
        //        collisionNormal = { 0, -1 };
        //        if (rectPtr->rect.y + rectPtr->rect.h / 2 > rectPtr2->rect.y + rectPtr2->rect.h / 2) {
        //            collisionNormal *= -1;
        //        }
        //    }
        //    collisionNormal *= -1;
        //    rectPtr2->ChangeBallDirection(collisionNormal);
        //    scoreLeft += 1;
        //    printf("Score left: %d", scoreLeft);

        //}
#pragma endregion Pong
        camera->Update();

        camera->Draw();


        for (auto& sticky : stickers) {
            sticky->Draw();
        }

        ball->Draw();
        //components[1]->Draw();
        //grid->Render(); 
        terrain->Render();

        for (auto& sticky : stickers) {
            sticky->Update(
                ball->transl,
                1.0f,
                ball->rotationAxis,
                ball->angle
            );;
        }

        context->OMSetRenderTargets(0, nullptr, nullptr);

        float currentHeight = terrain->GetHeightAt(ball->transl.x, ball->transl.z);
        ball->Update(camera->GetForwardDirection(), camera->GetCamPos(), currentHeight);
        //components[1]->Update();

        auto ballSphere = ball->GetBoundingSphere();

        for (auto& sticky : stickers) {
            if (sticky->mode == Mode::Attached) continue; // пропускаем уже прикреплённые

            auto box = sticky->GetBoundingBox();
            if (ballSphere.Intersects(box) == DirectX::ContainmentType::INTERSECTS) {
                std::cout << "Collided!" << std::endl;
                sticky->Attach(ball->transl, 1.0f, ball->angle, ball->rotationAxis);
            }
        }

        swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
    }
}


void Game::DestroyResources() {
    if (rtv) rtv->Release();
    if (swapChain) swapChain->Release();
    if (context) context->Release();
    //if (components[0]) {
    //    components[0]->DestroyResources();
    //    delete components[0];
    //}
}

std::vector<DirectX::XMFLOAT4> Game::GenerateCirclePoints(float cx, float cy, float r) {
    std::vector<DirectX::XMFLOAT4> points;
    const int numPoints = 32;
    const float angleStep = 2.0f * 3.1415926535f / numPoints;
    points.emplace_back(cx, cy, 0.5f, 1.0f);
    points.emplace_back(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

    for (int i = 0; i < numPoints; i++) {
        float angle = i * angleStep;
        float x = cx + r * cos(angle);
        float y = cy + r * sin(angle);
        points.emplace_back(x, y, 0.5f, 1.0f);
        points.emplace_back(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    return points;
}

std::vector<int> Game::GenerateCircleIndeces() {
    const int numPoints = 32;
    std::vector<int> indeces;
    for (int i = 1; i < numPoints; i++) {
        indeces.emplace_back(0);
        indeces.emplace_back(i);
        indeces.emplace_back(i + 1);
    }
    indeces.emplace_back(0);
    indeces.emplace_back(numPoints);
    indeces.emplace_back(1);
    
    return indeces;
}

DirectX::SimpleMath::Vector2 Game::GenerateRandomBallDirection()
{
    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

    DirectX::SimpleMath::Vector2 out;
    if (r < 0.5f) {
        out.x = -1;
    }
    else {
        out.x = 1;
    }
    r = (static_cast <float>(rand()) / static_cast <float>(RAND_MAX));
    out.y = r;
    out.Normalize();
    return out;
}

RectF GetRectangleProperties(const std::vector<DirectX::XMFLOAT4>& points) {
    float minX = points[0].x, maxX = points[0].x;
    float minY = points[0].y, maxY = points[0].y;

    for (size_t i = 0; i < points.size(); i += 2) {
        minX = (minX < points[i].x) ? minX : points[i].x;
        maxX = (maxX > points[i].x) ? maxX : points[i].x;
    }
    return { minX, maxY, maxX - minX, maxY - minY };
}

