#!/bin/bash
cd src/gui/frontend
npm run build
cd ../..
rm gui/backend/static
mkdir gui/backend/static
cp gui/frontend/dist/css gui/backend/static
cp gui/frontend/dist/js gui/backend/js
cp gui/frontend/dist/favicon.ico gui/backend/favicon.ico
cd ..

