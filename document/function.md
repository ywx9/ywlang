# Function

## `yw::ok`

### 1. displaying a message box with an OK button

#### Parameters

##### `string` text

text to display

##### `nullable<string>` caption

caption to display

#### Return

##### `bool`

## `yw::yesno`

### 1. displaying a message box with Yes and No buttons

#### Parameters

##### `string` text

text to display

##### `nullable<string>` caption

caption to display

#### Return

##### `bool`

## `yw::create_window`

### 1. creating a parent window

#### Parameters

##### `string` title

Title of the window.

##### `nullable<vector2>` position

Left-top position of the window.  

##### `nullable<vector2>` size

Width and height of the window.

##### `nullable<bool>` visible

Visibility of the window.

##### `nullable<>` frame

Flag to create window frames including title bar.

#### Return

##### `yw::window`

