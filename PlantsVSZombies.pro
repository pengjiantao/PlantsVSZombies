QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Info.cpp \
    InfoRead.cpp \
    PlantVSZombie.cpp \
    game.cpp \
    gamescene.cpp \
    main.cpp \
    plant.cpp \
    player.cpp \
    pumpkin.cpp \
    role_body.cpp \
    screen.cpp \
    settings.cpp \
    sun.cpp \
    swfunix.cpp \
    toolfunc.cpp \
    window.cpp

HEADERS += \
    Info.h \
    InfoRead.h \
    PlantVSZombie.h \
    game.h \
    gamescene.h \
    plant.h \
    player.h \
    pumpkin.h \
    role_body.h \
    screen.h \
    settings.h \
    sun.h \
    swfunix.h \
    toolfunc.h \
    window.h

TRANSLATIONS += \
    PlantsVSZombies_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    source.qrc

FORMS += \
    settings.ui \
    window.ui

DISTFILES += \
    image/Icon-120.png \
    image/Icon-144.png \
    image/audio/Grazy Dave.wav \
    image/audio/story_board.mp4 \
    image/bullet/PB-10.gif \
    image/bullet/PB00.gif \
    image/bullet/PB01.gif \
    image/bullet/PB10.gif \
    image/environment/0/meiguoxiaozhen.png \
    image/environment/1/haidaowan.png \
    image/environment/2/kuangyexibu.png \
    image/environment/card/_0049_Plant_Garlic.png \
    image/environment/card/_0060_snow-pea.png \
    image/environment/card/_0061_peashooter01.png \
    image/environment/card/_0066_squash01.png \
    image/environment/card/_0076_cherrybomb.png \
    image/environment/card/_0084_sunflower.png \
    image/environment/card/_0086_wallnut.png \
    image/environment/card/_0091_repeater.png \
    image/environment/card/gaojianguo.png \
    image/environment/card/nanguazhao.png \
    image/environment/nonstop/Background.jpg \
    image/environment/setting/background.jpg \
    image/environment/setting/options_checkbox0.png \
    image/environment/setting/options_checkbox1.png \
    image/files/forever.info \
    image/files/pass1.info \
    image/files/pass2.info \
    image/files/pass3.info \
    image/files/pass4.info \
    image/files/pass5.info \
    image/files/pass6.info \
    image/files/universal.info \
    image/plant/0/0.gif \
    image/plant/0/Peashooter.gif \
    image/plant/1/0.gif \
    image/plant/1/SunFlower1.gif \
    image/plant/1/SunFlower2.gif \
    image/plant/2/0.gif \
    image/plant/2/Repeater.gif \
    image/plant/3/0.gif \
    image/plant/3/SnowPea.gif \
    image/plant/4/0.gif \
    image/plant/4/1.gif \
    image/plant/4/2.gif \
    image/plant/4/BoomWallNutRoll.gif \
    image/plant/4/HugeWallNutRoll.gif \
    image/plant/4/WallNut.gif \
    image/plant/4/WallNutRoll.gif \
    image/plant/4/Wallnut_cracked1.gif \
    image/plant/4/Wallnut_cracked2.gif \
    image/plant/5/0.gif \
    image/plant/5/TallNut.gif \
    image/plant/5/TallnutCracked1.gif \
    image/plant/5/TallnutCracked2.gif \
    image/plant/6/0.gif \
    image/plant/6/0.png \
    image/plant/6/Squash.gif \
    image/plant/6/SquashAttack.gif \
    image/plant/6/SquashL.PNG \
    image/plant/6/SquashR.png \
    image/plant/7/0.gif \
    image/plant/7/Boom.gif \
    image/plant/7/CherryBomb.gif \
    image/plant/8/0.gif \
    image/plant/8/Garlic.gif \
    image/plant/8/Garlic_body2.gif \
    image/plant/8/Garlic_body3.gif \
    image/plant/9/0.gif \
    image/plant/9/PumpkinHead.gif \
    image/plant/9/PumpkinHead1.gif \
    image/plant/9/PumpkinHead2.gif \
    image/plant/9/Pumpkin_back.gif \
    image/plant/9/Pumpkin_damage2.gif \
    image/plant/9/pumpkin_damage1.gif \
    image/source/Almanac_IndexBack.jpg \
    image/source/LargeWave.gif \
    image/source/LogoWord.jpg \
    image/source/Shovel.png \
    image/source/Sun.gif \
    image/source/ZombiesWon.gif \
    image/source/ZombiesWon.png \
    image/source/trophy.gif \
    image/source/trophy.png \
    image/zombie/0/0.gif \
    image/zombie/0/1.gif \
    image/zombie/0/ConeheadZombie.gif \
    image/zombie/0/ConeheadZombieAttack.gif \
    image/zombie/1/0.gif \
    image/zombie/1/1.gif \
    image/zombie/1/BoomDie.gif \
    image/zombie/1/Die.gif \
    image/zombie/1/Head.gif \
    image/zombie/1/HeadAttack0.gif \
    image/zombie/1/HeadAttack1.gif \
    image/zombie/1/HeadWalk0.gif \
    image/zombie/1/HeadWalk1.gif \
    image/zombie/1/LostHeadAttack0.gif \
    image/zombie/1/LostHeadAttack1.gif \
    image/zombie/1/LostHeadWalk0.gif \
    image/zombie/1/LostHeadWalk1.gif \
    image/zombie/1/LostNewspaper.gif \
    image/zombie/2/0.gif \
    image/zombie/2/1.gif \
    image/zombie/2/BoomDie.gif \
    image/zombie/2/PoleVaultingZombie.gif \
    image/zombie/2/PoleVaultingZombieAttack.gif \
    image/zombie/2/PoleVaultingZombieDie.gif \
    image/zombie/2/PoleVaultingZombieHead.gif \
    image/zombie/2/PoleVaultingZombieJump.gif \
    image/zombie/2/PoleVaultingZombieJump2.gif \
    image/zombie/2/PoleVaultingZombieLostHead.gif \
    image/zombie/2/PoleVaultingZombieLostHeadAttack.gif \
    image/zombie/2/PoleVaultingZombieLostHeadWalk.gif \
    image/zombie/2/PoleVaultingZombieWalk.gif \
    image/zombie/3/0.gif \
    image/zombie/3/1.gif \
    image/zombie/3/Attack.gif \
    image/zombie/3/Boom.gif \
    image/zombie/3/BoomDie.gif \
    image/zombie/3/Die.gif \
    image/zombie/3/GoOut.gif \
    image/zombie/3/LostHead.gif \
    image/zombie/3/LostHeadAttack.gif \
    image/zombie/3/OpenBox.gif \
    image/zombie/3/Walk.gif \
    image/zombie/4/0.gif \
    image/zombie/4/1.gif \
    image/zombie/4/2.gif \
    image/zombie/4/3.gif \
    image/zombie/4/4.gif \
    image/zombie/4/5.gif \
    image/zombie/4/BoomDie.gif \
    image/zombie/4/ice.png \
    image/zombie/4/ice_cap.png \
    image/zombie/5/0.gif \
    image/zombie/5/1.gif \
    image/zombie/5/2.gif \
    image/zombie/5/3.gif \
    image/zombie/5/BoomDie.gif \
    image/zombie/5/Zombie.gif \
    image/zombie/5/Zombie2.gif \
    image/zombie/5/Zombie3.gif \
    image/zombie/5/ZombieAttack.gif \
    image/zombie/5/ZombieDie.gif \
    image/zombie/5/ZombieHead.gif \
    image/zombie/5/ZombieLostHead.gif \
    image/zombie/5/ZombieLostHeadAttack.gif
