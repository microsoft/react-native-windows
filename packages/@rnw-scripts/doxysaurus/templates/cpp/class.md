---
id: {{docId}}
title: {{name}}
---

Defined in `{{codeFileName}}`  
Namespace: **`{{namespace}}`**  
{{#namespaceAliases}}
Namespace alias: **`{{.}}`**
{{/namespaceAliases}}

## Definition

```cpp
{{&declaration}}
```

{{&summary}}

{{#sections}}

### {{name}}

Name | Description
-----|------------
{{#memberOverloads}}[**`{{&name}}`**]({{anchor}}) | {{&summary}}
{{/memberOverloads}}
{{/sections}}

{{#details}}

### Notes

{{&details}}
{{/details}}

{{#sections}}
{{#memberOverloads}}

---

## `{{&name}}`

{{#members}}

```cpp
{{&declaration}}
```

{{&brief}}

{{&details}}
{{/members}}

{{/memberOverloads}}
{{/sections}}
