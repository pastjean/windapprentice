![WindApprentice](https://raw.github.com/pastjean/windapprentrice/master/logo/logo.png)

Machine learning algorithms used in [Chinook][chinook] to control the pitch angle and the gearbox ratio. Some algorithms are also used to learn how the wind turbine acts in different conditions.

[chinook]:http://chinook.etsmtl.ca

Projects links
--------------

- [Wind Apprentice Trello board][trello]
- [Github project][gh_pro]
- [Chinook's github org][chinook_gh_org]

[gh_pro]:https://github.com/pastjean/windapprentice
[chinook_gh_org]:http://github.com/chinook
[trello]:https://trello.com/board/wind-apprentice/518a9dfd2d4ebe4e1a000b36


WindApprentice
--------------

Inputs:
-------

- Current Gear
- Current WindSpeed
- Current WindTurbine rpm

Output:
-------

- New Gear
- New Pitch Angle

Goal:
-----

The goal is to find the best pitch and gear to put the wind turbine on so we can get more power from it. The fitness function is defined from a Levenberg-Marquardt parametric model regression (cp_finder).
