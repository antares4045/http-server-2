import React, { Component, useReducer , useState} from "react";


import { DragDropContext, Droppable, Draggable } from "react-beautiful-dnd";


const reorder = (list, startIndex, endIndex) => {
  const result = Array.from(list);
  const [removed] = result.splice(startIndex, 1);
  result.splice(endIndex, 0, removed);
  return result;
};

const grid = 8;

const getItemStyle = (isDragging) => ({
  // some basic styles to make the items look a bit nicer
  userSelect: "none",
  padding: grid * 2,
  margin: `0 0 ${grid}px 0`,
  // change background colour if dragging
  background: isDragging ? "lightgreen" : "grey",

});

const getListStyle = isDraggingOver => ({
  background: isDraggingOver ? "lightblue" : "lightgrey",
  padding: grid,
  width: 250
});


export default function DNDList({items, setItems, getListStyle, getItemStyle, content, itemClickHandler=()=>{}}) {


  function onDragEnd(result) {
      console.log(result)
     if (!result.destination) {
      return;
    }
    setItems(reorder(items,
        result.source.index,
      result.destination.index))

  }

  return (
     <DragDropContext onDragEnd={onDragEnd}>
        <Droppable droppableId="droppable">
          {(provided, snapshot) => (
            <div
              {...provided.droppableProps}
              ref={provided.innerRef}
              style={getListStyle(snapshot.isDraggingOver)}
            >
              {items.map((item, index) => (
                <Draggable key={item.id} draggableId={item.id} index={index}>
                  {(provided, snapshot) => {
                      
                     return <div
                      onClick={(event) => itemClickHandler(item, event)}
                      ref={provided.innerRef}
                      {...provided.draggableProps}
                      {...provided.dragHandleProps}
                      style={{...getItemStyle(
                        snapshot.isDragging, item
                      ), ...provided.draggableProps.style}}
                    >
                      {content(item, index)}
                    </div>

                  }
                  }
                </Draggable>
              ))}
              {provided.placeholder}
            </div>
          )}
        </Droppable>
      </DragDropContext>
  );
}
