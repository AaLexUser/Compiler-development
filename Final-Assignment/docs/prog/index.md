## Получившаяся грамматика
**prog:**

![prog](../diagram/prog.png)

**flow:**

![flow](diagram/flow.png)

referenced by:

* list

**if_stmt:**

![if_stmt](diagram/if_stmt.png)

referenced by:

* flow

**else_stmt:**

![else_stmt](diagram/else_stmt.png)

referenced by:

* if_stmt

**list:**

![list](diagram/list.png)

referenced by:

* else_stmt
* flow
* if_stmt
* prog

**exp:**

![exp](diagram/exp.png)

referenced by:

* exp
* flow
* if_stmt
* list

