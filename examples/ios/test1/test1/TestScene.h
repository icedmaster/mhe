//
//  TestScene.h
//  test1
//
//  Created by skraevsky on 05.12.12.
//  Copyright (c) 2012 mht. All rights reserved.
//

#ifndef __test1__TestScene__
#define __test1__TestScene__

#include <mhe.hpp>

class TestScene : public mhe::game::GameScene
{
public:
    TestScene(mhe::game::Engine* engine):
        mhe::game::GameScene(engine)
    {}
    
    bool init_impl(const std::string&, void*);
};

#endif /* defined(__test1__TestScene__) */
