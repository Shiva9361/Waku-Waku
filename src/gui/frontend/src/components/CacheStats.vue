<template>
  <div>
    <h4 style="margin: 5px 0px 5px 0px">Cache Stats</h4>
    <div v-for="(value, key) in stats" :key="key">
      <div class="stats">
        <div>{{ key }}:&nbsp;</div>
        <div>{{ value }}</div>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: "CacheStatsComponent",
  props: {
    stats: Object,
  },
};
</script>

<style>
.stats {
  display: flex;
}
</style>
