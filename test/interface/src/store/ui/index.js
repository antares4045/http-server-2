import { createContext, useContext, useEffect } from 'react'
import createProvider from '../provider'


import state from './state'
import mutations from './mutations'
import getters from './getters'
import providers from './providers'


const storage = {
    state, mutations, getters, providers
}

const Context = createContext([{}, () => {}]);

export default () => useContext(Context)

export const Provider = ({children}) => createProvider({children, storage, Context})