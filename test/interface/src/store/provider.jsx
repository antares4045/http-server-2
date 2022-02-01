import React, { useReducer } from 'react'



const reducerFactory = ({mutations={}, getters={}}, providers) => 
    (state, { type, payload }) => {
  if (type in mutations) {
    //console.log('commiting', type, payload, '=>', state);
    let newState = { ...state };
    mutations[type](
      {
        state: newState,
        commit: (type, payload) =>
          (newState = reducerFactory({mutations, getters}, providers)(newState, { type, payload })),
        getter: (type, payload) => getterFactory({getters}, providers)({ state }, { type, payload })
      },
      payload
    );
    //console.log('commited ', type, payload, '=>', newState);
    return newState;
  } else if(type instanceof Array){
    
    const provider = providers[type[0]]
    
    if(!provider){
      throw `Не известный провайдер "${type[0]}"`;
    }
    provider[1][type[1]](payload)


  }else {
    throw `Не известная мутация "${type}"`;
  }
};


const initializeMutations = (commit, {mutations={}}) => {
  return Object.fromEntries(
    Object.keys(mutations).map(key => {
      return [key, payload => commit({ type: key, payload })];
    })
  );
};


const getterFactory = ({getters}, providers) => ({ state }, { type, payload }) => {
  if(type in getters)
    return getters[type](
      {
        state,
        getter: (type, payload) => getterFactory({getters}, providers)({ state }, { type, payload })
      },
      payload 
    );
  if(type instanceof Array){
    const provider = providers[type[0]]
    if(!provider)
      throw `Не известный провайдер "${type[0]}"`;
    if(!(type[1] in provider[2]))
      throw `провайдер "${type[0]}" не содержит геттер ${type[1]}`
    return provider[2][type[1]](payload)

  }
};

const initializeGetters = (state, {getters={}}, providers) => {
  return Object.fromEntries(
    Object.keys(getters).map(type => {
      return [type, payload => getterFactory({getters}, providers)({ state }, { type, payload })];
    })
  );
};


const ProviderFactory = ({storage={}, Context, children},) => {
  

  const providers = Object.fromEntries(
    Object.keys(storage.providers || {}).map(key => [key, storage.providers[key]()]))


  const [state, commit] = useReducer(reducerFactory(storage, providers), storage.state || {});

  const initedMutators = initializeMutations(commit, storage);

  const initedGetters = initializeGetters(state, storage, providers);
  /* eslint-disable no-unused-expressions */
  (storage.use ? storage.use([state, initedMutators, initedGetters]): null)

  return (
    <Context.Provider
      value={[state, initedMutators, initedGetters]}
    >
      {children}
    </Context.Provider>
  );
}


export default ProviderFactory