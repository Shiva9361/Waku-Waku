<template>
  <div>
    <div v-for="(value, key) in stats" :key="key">
      <div>{{ key }}</div>
      <div>{{ value }}</div>
    </div>
  </div>
</template>

<script>
export default {
  name: "StatsComponent",
  props: {
    stats: Object,
  },
};
</script>

<style></style>
