import path from 'node:path'
import tailwindcss from '@tailwindcss/vite'
import react from '@vitejs/plugin-react'
import { defineConfig } from 'vite'

export default defineConfig({
  // Relative asset URLs: the build is served by the native host from a virtual host, not from the site root.
  base: './',
  plugins: [react(), tailwindcss()],
  resolve: { alias: { '@': path.resolve(__dirname, './src') } },
  build: {
    // The bundle is loaded from disk by a desktop app, so the usual "keep chunks small for the network" warning does not apply.
    chunkSizeWarningLimit: 800,
  },
})
